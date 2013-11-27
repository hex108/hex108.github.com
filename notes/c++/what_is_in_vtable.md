---
layout: note 
title: 虚表里有些什么
---

{{ page.title }}
================

有了虚继承以后，虚表(virtual table)里就不只包含虚函数了。[Itanium C++ ABI](http://mentorembedded.github.io/cxx-abi/abi.html#vtable)上对虚表的描述如下：

> 虚表的作用：

> A virtual table (vtable) is a table of information used to dispatch virtual functions, to access virtual base class subobjects, and to access information for runtime type identification (RTTI). 

> 虚表里有些什么：

> A virtual table consists of a sequence of offsets, data pointers, and function pointers, as well as structures composed of such items.

> 什么样的类会有虚表：

> Each class that has virtual member functions or virtual bases has an associated set of virtual tables. There may be multiple virtual tables for a particular class, if it is used as a base class for other classes. However, the virtual table pointers within all the objects (instances) of a particular most-derived class point to the same set of virtual tables.

按照和[内存布局：虚函数表](data_layout.html)相同的结构组织了本文。代码依旧用G++ 4.6.3编译的。

## 1. 单继承（与多重继承对应）
类的定义如下所示：
{% highlight c++ %}
class A{
public:
    int ia;
    virtual void foo(){ cout << "A::foo()" << endl; }
    virtual void bar(){ cout << "A::bar()" << endl; }
};
class B: public A{
public:
    int ib;
    virtual void foo(){ cout << "B::foo()" << endl; }
};
{% endhighlight %}

我们可以利用`g++ -fdump-class-hierarchy vtable.cpp`得到`class A`和`class B`的`vtable`（结果在文件`vtable.cpp.002t.class`里），如下所示：
{% highlight c++ %}
Vtable for A
A::_ZTV1A: 4u entries
0     (int (*)(...))0
4     (int (*)(...))(& _ZTI1A)
8     (int (*)(...))A::foo
12    (int (*)(...))A::bar

Vtable for B
B::_ZTV1B: 4u entries
0     (int (*)(...))0
4     (int (*)(...))(& _ZTI1B)
8     (int (*)(...))B::foo
12    (int (*)(...))A::bar
{% endhighlight %}

我们注意到上面的`vtable`里各有4项，最后2项是2个虚函数指针，第一项是**offset to top**，第二项是**typeinfo pointer**，这两项都与[RTTI](http://en.wikipedia.org/wiki/Run-time_type_information)有关，我将在另一篇文章[RTTI的实现细节](rtti.html)里解释这两项的作用。

## 2. 多重继承
类的定义如下所示：
{% highlight c++ %}
class A{
public:
    int ia;
    virtual void foo(){ cout << "A::foo()" << endl; }
    virtual void bar(){ cout << "A::bar()" << endl; }
};
class B{
public:
    int ib;
    virtual void foo2(){ cout << "B::foo2()" << endl; }
    virtual void bar2(){ cout << "B::bar2()" << endl; }
};
class C: public A, public B{
public:
    int ic;
    virtual void foo(){ cout << "C::foo()" << endl; }
    virtual void foo2(){ cout << "C::foo2()" << endl; }
};
{% endhighlight %}

`class A, B, C`的`vtable`分别为：
{% highlight c++ %}
Vtable for A
A::_ZTV1A: 4u entries
0     (int (*)(...))0
4     (int (*)(...))(& _ZTI1A)
8     (int (*)(...))A::foo
12    (int (*)(...))A::bar

Vtable for B
B::_ZTV1B: 4u entries
0     (int (*)(...))0
4     (int (*)(...))(& _ZTI1B)
8     (int (*)(...))B::foo2
12    (int (*)(...))B::bar2

Vtable for C
C::_ZTV1C: 9u entries
0     (int (*)(...))0
4     (int (*)(...))(& _ZTI1C)
8     (int (*)(...))C::foo
12    (int (*)(...))A::bar
16    (int (*)(...))C::foo2
20    (int (*)(...))-0x00000000000000008
24    (int (*)(...))(& _ZTI1C)
28    (int (*)(...))C::_ZThn8_N1C4foo2Ev
32    (int (*)(...))B::bar2
{% endhighlight %}

`A`和`B`的`vtable`没有什么异常，不过`C`的`vtable`有个地方和我想像的不同，我觉得在`_ZTV1C + 20`的地址处应试有个`B::bar2`，这样`C *p = &c; p->bar2()`就能和`p->foo2()`生成类似的机器指令，也算是相同的接口吧。现在在这个地址处没有`B::bar2`，因此`p->bar2()`翻译成了如下的指令，它需要从`C`里所包含的`B`的结构里去寻找`B::bar2`。我觉得该地址处没有`B::bar2`是为了避免重复，如果`B`中有`N`个类似`B::bar2`的函数，则需要重复`N`次，类结构复杂时还会有更多的重复。
{% highlight c++ %}
(gdb) n
32	    p->bar2();
(gdb) x/10i $pc
=> <main()+68>:  mov    0x3c(%esp),%eax  // 把p的值取出来放到eax中
   <main()+72>:  mov    0x8(%eax),%eax   // (eax + 8)里的内容是vptr.B的值(_ZTV1C + 28)
   <main()+75>:  add    $0x4,%eax  // eax + 4后，指向 _ZTV1C + 32，即指向 B::bar2
   <main()+78>:  mov    (%eax),%eax
   <main()+80>:  mov    0x3c(%esp),%edx
   <main()+84>:  add    $0x8,%edx  // edx 指向 vptr.B所在的地址，它正好是B的成员变量在C中的起始地址
   <main()+87>:  mov    %edx,(%esp) // 其实相当于传的是this指针，B::bar2将会和访问类型为B的对象一样
   <main()+90>:  call   *%eax  // call B::bar2
{% endhighlight %}

注：`C c; c.bar2();`被翻译成机器指令后直接是`call B::bar2()`，当然在`call`之前也需要像上面一样调整`this`指针，指向`vptr.B`。

为什么`_ZTI1C`在`C`的`vtable`中出现了两次（`_ZTV1C + 4`和`_ZTV1C + 24`）？这也和[RTTI](http://en.wikipedia.org/wiki/Run-time_type_information)有关，我将在[RTTI的实现细节](rtti.html)中讨论它。

## 3. 虚继承
类的定义如下所示：
{% highlight c++ %}
class A{
public:
    int ia;
    virtual void foo(){ cout << "A::foo()" << endl; }
    virtual void bar(){ cout << "A::bar()" << endl; }
};
class B: public virtual A{
public:
    int ib;
    virtual void foo(){ cout << "B::foo()" << endl; }
};
{% endhighlight %}

各个类的`vtable`如下所示：
{% highlight c++ %}
Vtable for A
A::_ZTV1A: 4u entries
0     (int (*)(...))0
4     (int (*)(...))(& _ZTI1A)
8     (int (*)(...))A::foo
12    (int (*)(...))A::bar

Vtable for B
B::_ZTV1B: 10u entries
0     8u
4     (int (*)(...))0
8     (int (*)(...))(& _ZTI1B)
12    (int (*)(...))B::foo
16    0u
20    4294967288u
24    (int (*)(...))-0x00000000000000008
28    (int (*)(...))(& _ZTI1B)
32    (int (*)(...))B::_ZTv0_n12_N1B3fooEv
36    (int (*)(...))A::bar

VTT for B
B::_ZTT1B: 2u entries
0     ((& B::_ZTV1B) + 12u)
4     ((& B::_ZTV1B) + 32u)
{% endhighlight %}

我们注意到`class B`多了一个`VTT`，它的定义如下。

> An array of virtual table addresses, called the VTT, is declared for each class type that has indirect or direct virtual base classes. 

`class B`的`VTT`里的内容是`vptr.B`和`vptr.A`，正好验证了上面对`VTT`的定义。`VTT`的作用是什么呢？硬着头皮看ABI。

此外，我们看看`B::_ZTV1B + 0`处的`8u`和`B::_ZTV1B + 24`处的`-0x00000000000000008`，它们的作用是"to access virtual base class subobjects"。举例如下：假如`class B`中有一个函数`int foo2(){ return ia; }`，想访问基类`A`中的成员变量 `ia`，编译器该怎么实现`foo2`呢？如下所示：
{% highlight c++ %}
=> 0x80488ae <B::foo2()>:    push   %ebp     
   0x80488af <B::foo2()+1>:  mov    %esp,%ebp
   0x80488b1 <B::foo2()+3>:  mov    0x8(%ebp),%eax  // this指针
   0x80488b4 <B::foo2()+6>:  mov    (%eax),%eax // vptr.B （它的值 = B::_ZTV1B + 12）
   0x80488b6 <B::foo2()+8>:  sub    $0xc,%eax  // B::_ZTV1B + 0
   0x80488b9 <B::foo2()+11>: mov    (%eax),%eax // 8u
   0x80488bb <B::foo2()+13>: add    0x8(%ebp),%eax // this + 8u（它的值指向vptr.A了）
   0x80488be <B::foo2()+16>: mov    0x4(%eax),%eax // eax指向ia了
   0x80488c1 <B::foo2()+19>: pop    %ebp
   0x80488c2 <B::foo2()+20>: ret  
{% endhighlight %}

通过[内存布局：虚函数表](data_layout.hmtl)我们了解了`class B`的内存布局如下所示，`sizeof(vptr.B) + sizeof(ib) = 8`，通过上面代码的注释我们就能知道`B::_ZTV1B + 0`处的`8u`是`class B`用来访问其内部的`class A`的，同理我们可知道，`B::_ZTV1B + 24`处的`-0x00000000000000008`是`class A`（特指`class B`内部的`class A`）用来访问其外部的`class B`的。

    vptr.B | ib | vptr.A | ia

至于`B::_ZTV1B + 16`和`B::_ZTV1B + 20`这两项内容，我暂时还没搞清楚它们有什么作用。根据[C++ Vtable Example](http://mentorembedded.github.io/cxx-abi/cxx-vtable-ex.html) Table 1c中的例子，我们推测它们俩分别是`offset for bar`，`offset for foo`(4294967288u的十六进制值为0xfffffff8，即-8)，不过依旧不知道它们有什么作用。

## 4. 小结
现在，我们终于可以理解[C++ Vtable Example](http://mentorembedded.github.io/cxx-abi/cxx-vtable-ex.html) Table 1c了。