---
layout: post
title: GDB如何获取虚基类中成员变量的偏移
category: GDB 
summary: GDB可以通过一个真正的类实例才能获取虚基类中成员变量的偏移，但是通过NULL Pointer是不行的。
tags: [C++, virtual class, GDB]
---

{{ page.title }}
================

{{ page.summary }}

## 问题
我们希望在系统运行时运行一些基于GDB的Debug Extention，以便获取系统运行时的一些信息。其中一个API是获取成员变量在结构体(struct/class)中的偏移，这个API在一般的情况下能正常工作，不过遇到虚基类(virtual base class)就有问题了。

把问题进行简化，可以得到如下测试例子：

{% highlight c++ %}
// test.cpp
class A{
public:
    int a;
};

class B: public virtual A{
public:
    int b;
};

int main(){
    B t;

    return 0;
}
{% endhighlight %}

{% highlight c++ %}
$ gdb ./test
(gdb)p &(((B * )0)->a)
Cannot access memory at address 0x0
{% endhighlight %}

## 分析
### GDB如何存储`class A`和`class B`
通过`readelf -wi test`可以查看DWARF文件(用-g选项编译出的test文件)是如何存储`A`和`B`的，GDB只是把它们读出来并进行了一些处理，基本的关系不会变。

    B ----- b (int, offset: 4)              <---  B's own members
        |
        --- A (public, virutal, offset: ?)  <---  B's base classes

    A ----- a (int, offset: 0)

### GDB怎么访问虚基类成员`a`
`p &(((B *)0)->a)`在GDB中经历了如下几步：

1.查找自己的成员变量

查找`class B`自己的成员变量中是否有`a`，没找到则会依次查找其基类。

2.查找基类

`B`的基类只有`A`，因此查找`A`。通过获取`a`在`A`中偏移，再加上`A`在`B`中的偏移，即是`a`在`B`中的偏移。`A`是`B`的虚基类, 要通过`B`中的virtual table才能获得`A`在`B`中的偏移(这是[C++ ABI](http://mentorembedded.github.io/cxx-abi/abi.html#vtable)规定的)。
> Virtual Base (vbase) offsets are used to access the virtual bases of an object. Such an entry is added to the derived class object address (i.e. the address of its virtual table pointer) to get the address of a virtual base class subobject.

此时，通过地址0去访问virtual table，则会出错。如果这个地址是合法的，就能正常 访问，比如:


{% highlight c++ %}
(gdb) p &(((B *)&t)->a)
$1 = (int *) 0xbffff2fc
(gdb) p (char *)&(((B *)&t)->a) - (char *)&t  // 获得a在B中的偏移
$2 = 8
{% endhighlight %}

## 后记
### 能通过别的方式获得`A`在`B`中的偏移吗
我觉得是可以的，可以见GDB mailist上的[讨论](https://sourceware.org/ml/gdb/2013-09/msg00030.html)。编译器在编译时已经确定了`A`在`B`中的偏移，但是它并没有将该偏移写入test文件中（该偏移值在`B`的复制构造函数中可以看到)，因此GDB也无能为力了。当然，这个偏移值不是必须的，通过virtual table就可以得到，也不是ABI要求的。对于多态,这个偏移值也是没有意义的, 例如: 如果定义`class C: public B: public virtual A{}`，申明`C t2`, `&(((B *)&t2)->a)`的偏移和上述`A`在`B`的偏移是没有必然关系的。

### 通过virtual table才能得到虚基类的偏移有什么优点
* 如果没有多态, 直接通过偏移值访问就可以了，而且更高效。

* 有多态时, 编译器只能通过virtual table去得到偏移。例如：
{% highlight c++ %}
B t;
C t2;
t.a = 3;
t2.a = 4;
B *p1 = &t;
B *p2 = (B *)&t2;
cout << p1->a << endl;
cout << p2->a << endl;
{% endhighlight %}

编译器生成二进制代码时只能通过virtual table去获取a在p1和p2中的偏移。
