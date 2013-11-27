---
layout: note 
title: 内存布局：虚函数表
---

{{ page.title }}
================

关于C++内存布局的ABI可以见[Itanium C++ ABI](http://mentorembedded.github.io/cxx-abi/abi.html)（ABI都是这么枯燥吗？）。内存布局的问题挺多的：这篇文章主要想讲清楚两个问题：

* 虚函数表(virtual table)怎么存储的
* 当把子类对象的地址赋值给父类指针时，父类指针指向哪

首先说明一下"子类和父类内存布局一致"的原则。以`class Base`和`class Derived: public Base`为例说明，`Derived`对象的布局应该让从`Base`继承的成员变量的布局和`Base`中的布局一样，否则需要调整（见下文"虚继承"）。原因如下：对于下面的代码`Base *p = new Derived()`，如果调用`p->foo()`（`foo`为`Base`的成员函数，不是虚函数），那么`p->foo()`将调用的是`Base`中的`foo`函数，因此会按照`Base`的内存布局对`p`指向的内存进行操作。

下面我们用GDB看看它们是怎么布局的（代码用G++ 4.6.3编译）。

## 1. 单继承（与多重继承对应） 
几个class的定义如下所示（完整代码为[data_layout](code/data_layout.txt)）：
{% highlight c++ %}
class A{
public:
    int a;
    virtual void foo(){ cout << "A::foo()" << endl; }
    void bar(){ cout << "A::bar()" << endl; }
};
class B: public A{
public:
    int b;
    void foo(){ cout << "B::foo()" << endl; }
};
class C: public B{
public:
    int c;
    void foo(){ cout << "C::foo()" << endl; }
};
{% endhighlight %}

代码中有几个对象`A a; B b; C c; B *p = &c`，我们分别看看它们在内存中的值：
{% highlight c++ %}
(gdb) p /x a
$1 = {_vptr.A = 0x8048990, a = 0x8048879}
(gdb) x/2a &a
0xbffff2d4:	0x8048990 <_ZTV1A+8>	0x8048879 <__libc_csu_init+9>
(gdb) p /x b
$2 = {<A> = {_vptr.A = 0x8048980, a = 0xb7d6e225}, b = 0xb7fed280}
(gdb) x/3a &b
0xbffff2c8:	0x8048980 <_ZTV1B+8>	0xb7d6e225 <__cxa_atexit+53>	0xb7fed280
(gdb) p /x c
$3 = {<B> = {<A> = {_vptr.A = 0x8048970, a = 0x80488c2}, b = 0x1}, 
  c = 0xbffff384}
(gdb) x/4a &c
0xbffff2b8:	0x8048970 <_ZTV1C+8>	0x80488c2 <__libc_csu_init+82>	0x1	0xbffff384
{% endhighlight %}

从上面的数据可以知道它们的内存布局分别为：

    a:   vptr.A | a
    b:   vptr.A | a | b
    c:   vptr.A | a | b | c
 
vptr.A是一个指向虚函数表的指针，它只占一个指针大小的空间。通过在命令行运行`c++filt _ZTV1A`可以知道_ZTV1A的意思是“vtable for A”。**每个类都有自己的虚函数表（当然前提是该类里有虚函数）**，所以对象`a`, `b`, `c`里所存的虚函数表是各不相同的，如下所示：
{% highlight c++ %}
(gdb) x/a 0x8048990
0x8048990 <_ZTV1A+8>:	0x804877a <A::foo()>
(gdb) x/a 0x8048980
0x8048980 <_ZTV1B+8>:	0x80487a6 <B::foo()>
(gdb) x/a 0x8048970
0x8048970 <_ZTV1C+8>:	0x80487fe <C::foo()>
{% endhighlight %}

该虚函数表里不包含函数`bar`。如果把bar声明为`virtual`，则会包含在虚函数表里。
{% highlight c++ %}
0x8048a00 <_ZTV1A+8>:	0x8048782 <A::foo()>	0x80487ae <A::bar()>
{% endhighlight %}

单继承的布局就说完了，其实还有好些其他问题，不过限于篇幅，可以自己动手试试，比如：

* 如果`class B`中申明了新的虚函数（比如`foo2`），`class B`中依然只有一个虚函数表，只不过会把`foo2`加入到该表中。此时`class A`的虚函数表不会包含`foo2`。
* 如果`class A`中的`foo`没有声明为`virtual`，在`class B`中，`foo`才会被声明为`virtual`，那么`class A`将不会有虚函数表。它们的内存布局分别为：

      a:   a
      b:   vptr.B | a | b
      c:   vptr.B | a | b | c

  此时如果我们定义`A *p = &b；`，那么`p`将指向`b.a`所在的地址。

## 2. 多重继承
把上面3个类的定义改为:
{% highlight c++ %}
class A{
    ...
    virtual void foo(){ cout << "A::foo()" << endl; }
};
class B{
    ...
    virtual void bar(){ cout << "B::bar()" << endl; }
};
class C: public A, public B{
    ...
    void foo(){ cout << "C::foo()" << endl; }
    void bar(){ cout << "C::bar()" << endl; }
};
{% endhighlight %}

三个对象的内存布局分别为：

    a:   vptr.A | a
    b:   vptr.B | b
    c:   vptr.A | a | vptr.B | b | c

对于`c`的布局需要着重说明一下，如下所示：
{% highlight c++ %}
(gdb) p /x c
$4 = {<A> = {_vptr.A = 0x80489b8, a = 0x1}, <B> = {_vptr.B = 0x80489c8, 
    b = 0x1}, c = 0xbffff384}
(gdb) x/5a &c
0xbffff2b4:	0x80489b8 <_ZTV1C+8>	0x1	0x80489c8 <_ZTV1C+24>	0x1
0xbffff2c4:	0xbffff384
(gdb) x/2a 0x80489b8
0x80489b8 <_ZTV1C+8>:	0x80487e2 <C::foo()>	0x8048816 <C::bar()>
{% endhighlight %}

`c`里的`vptr.A`所指向的虚函数表包括了`c`中的所有虚函数，这样对于类型为C或C的子类的对象/对象指针，当它们调用虚函数时，就不用进行特殊处理了（如：调整指针偏移），此时它们并不需要知道还有`vptr.B`的存在。对于`B *pb = &c; pb->bar();`，`pb`需要指向`vptr.B`，这样才能保证**`pb`指向`&c`和`pb`指向`&b`的内存环境是一样的**。

现在我们看看为什么`vptr.B`的地址为什么不是`0x8048816 <C::bar()>`而是`0x80489c8 <_ZTV1C+24>`？
{% highlight c++ %}
(gdb) x/2a 0x80489c8
0x80489c8 <_ZTV1C+24>:	0x804880e <_ZThn8_N1C3barEv>	0x0
(gdb) x/3i 0x804880e
   0x804880e <_ZThn8_N1C3barEv>:    subl   $0x8,0x4(%esp)
   0x8048813 <_ZThn8_N1C3barEv+5>:  jmp    0x8048816 <C::bar()>
   0x8048815:	nop
{% endhighlight %}

`_ZThn8_N1C3barEv`的意思是`non-virtual thunk to C::bar()`，在这个函数里`subl   $0x8,0x4(%esp)`的作用是把第一个参数的值减`8`。当对象调用成员函数时，编译器会默认地将`this`指针当作第一个参数传进去。需要减`8`的原因是：`B *pb = &c`时，`pb`的值是调整过的值`&c + 8`，它指向`vptr.B`，而此时调用函数`bar`，该函数需要解析的是对象`c`，这样才能完整地实现多态（调用`c`的函数，解析的对象也是`c`），因此需要传进去的第一个参数是`&c` `=` `pb - 8`。

思考：如果此时定义`class D`为`class D: public C`，并且定义了新的虚函数表，那么`class D`的对象的内存布局是怎样的呢？

## 3. 虚继承
把上面3个类加上一个新增加的类`D`的定义改为：
{% highlight c++ %}
class A{
    ...  
    virtual void foo(){ cout << "A::foo()" << endl; }
};
class B: virtual public A{
    ...
    virtual void foo(){ cout << "B::foo()" << endl; }
};
class C: virtual public A{
    ...
    virtual void foo(){ cout << "C::foo()" << endl; }
};
class D: public B, public C{
    ...
    virtual void foo(){ cout << "D::foo()" << endl; }
};
{% endhighlight %}

四个类的对象的内存布局分别为：

    a:   vptr.A | a
    b:   vptr.B | b | vptr.A | a
    c:   vptr.C | c | vptr.A | a
    d:   vptr.B | b | vptr.C | c | d | vptr.A | a

`A *pa = &d;`，`pa`指向的是`vptr.A`。`B *pb = &d;`，`pb`指向的是`vptr.B`。`C *pc = &d;`，`pc`指向的是`vptr.C`。调用`pa->foo()`和`pc->foo()`时需要像本文上面所说的一样对第一个参数`this`指针进行调整，而`pb->foo()`不需要。

但是这个内存布局违反了本文最开始说的"子类和父类内存布局一致"的原则。如何解决这个问题？在虚函数表中将会有一项能帮助`pb->foo()`去找到`vptr.A`的地址。更详细的解释可以见另一篇文章《[虚表里有些什么](what_is_in_vtable.html)》，简单的解释如下所示（引自[Itanium C++ ABI](http://mentorembedded.github.io/cxx-abi/abi.html)）：

> A virtual table consists of a sequence of offsets, data pointers, and function pointers, as well as structures composed of such items. 

## 4. 小结
辛苦地写了这么久，发现有一个表([C++ Vtable Example](http://mentorembedded.github.io/cxx-abi/cxx-vtable-ex.html) Table 1b)已经很好地总结了我所想表达的，不过它只有结果，看完本文就能较好地理解这个表了。