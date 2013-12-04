---
layout: note 
title: 函数重载与虚函数
---

{{ page.title }}
================

函数重载与虚函数的区别在于：调用重载函数时编译时就能确定其调用地址，而当用对象指针调用虚函数时必须通过虚表去调用。

举例说明，类的定义如下示：
{% highlight c++ %}
class A{
public:
    virtual void foo(){ cout << "A::foo()" << endl; }
    void bar(){ cout << "A::bar()" << endl; }
};
class B: public A{
public:
    virtual void foo(){ cout << "B::foo()" << endl; }
    void bar(){ cout << "B::bar()" << endl; }
};
{% endhighlight %}

`B b; A *p = &b;`, `p->bar()`对应的汇编码为：
{% highlight c++ %}
=> 0x80486e1 <main()+29>:	mov    0x1c(%esp),%eax
   0x80486e5 <main()+33>:	mov    %eax,(%esp)
   0x80486e8 <main()+36>:	call   0x804878e <A::bar()>
{% endhighlight %}

在编译时`p`的类型为`A *`，对于重载函数，不管它是否指向子类成员，编译器都会调用`A`定义的函数`bar`，即`A::bar()`。

`p->foo()`对应的汇编码为：
{% highlight c++ %}
=> 0x80486ed <main()+41>:	mov    0x1c(%esp),%eax
   0x80486f1 <main()+45>:	mov    (%eax),%eax
   0x80486f3 <main()+47>:	mov    (%eax),%edx
   0x80486f5 <main()+49>:	mov    0x1c(%esp),%eax
   0x80486f9 <main()+53>:	mov    %eax,(%esp)
   0x80486fc <main()+56>:	call   *%edx
{% endhighlight %}