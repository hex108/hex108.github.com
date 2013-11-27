---
layout: note 
title: RTTI的实现细节
---

{{ page.title }}
================

在[虚表里有些什么](what_is_in_vtable.html)里面我们提到了`vtable`里和[RTTI](http://en.wikipedia.org/wiki/Run-time_type_information)有关的两项`offset to top`，`typeinfo pointer`。[Itanium C++ ABI](http://mentorembedded.github.io/cxx-abi/abi.html#vtable)对它们的描述为：

> The **offset to top** holds the displacement to the top of the object from the location within the object of the virtual table pointer that addresses this virtual table, as a ptrdiff_t. It is always present. The offset provides a way to find the top of the object from any base subobject with a virtual table pointer. This is necessary for dynamic_cast in particular.
 (In a complete object virtual table, and therefore in all of its primary base virtual tables, the value of this offset will be zero. [...])

> The **typeinfo pointer** points to the typeinfo object used for RTTI. It is always present. All entries in each of the virtual tables for a given class must point to the same typeinfo object. A correct implementation of typeinfo equality is to check pointer equality, except for pointers (directly or indirectly) to incomplete types. The typeinfo pointer is a valid pointer for polymorphic classes, i.e. those with virtual functions, and is zero for non-polymorphic classes.

## 1. typeid

我们先看看`typeinfo pointer`，它和C++中的操作符[typeid](http://en.cppreference.com/w/cpp/language/typeid)有着很重要的联系。

> In C++, the typeid keyword is used to determine the class of an object at run time. It returns a reference to std::type_info object, which exists until the end of the program.
 -- From [Wiki](http://en.wikipedia.org/wiki/Typeid)

通过`typeid`得到`std::type_info object`是很简单的。如果是`typeid`的参数的类型是编译时就能确定的（不是多态），那么在编译时就能得到对应的`type_info`。如果`typeid`的参数的类型是多态的，则需要动态地去获取相应的`type_info`，例如：`Base *p = new Derived(); const type_info &type = typeid(*p)`对应的汇编如下所示：
{% highlight c++ %}
=> 0x8048779 <main()+53>:	cmpl   $0x0,0x44(%esp)        // p是否为空指针nullptr
   0x804877e <main()+58>:	je     0x80487d9 <main()+149>
   0x8048780 <main()+60>:	mov    0x44(%esp),%eax     // eax = *p
   0x8048784 <main()+64>:	mov    (%eax),%eax    // p指向的是vptr.Derived，eax将指向虚函数表
   0x8048786 <main()+66>:	mov    -0x4(%eax),%eax // 虚函数表上一项即是指向`type_info`的结构
   0x8048789 <main()+69>:	mov    %eax,0x48(%esp)
{% endhighlight %}

以[虚表里有些什么](what_is_in_vtable.html)第二小节“多重继承”里的`class C`为例，`class C`的`vtable`如下所示，`vptr.A`指向`C::_ZTV1C + 8`，`vptr.B`指向`C::_ZTV1C + 28`，它们的上一项都指向`& _ZTI1C`，即`class C`对应的`type_info object`。
{% highlight c++ %}
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

## 2. dynamic_cast
`offset to top`和`dynamic_cast`有直接的关系。[Understanding the vtable entries](http://stackoverflow.com/questions/5712808/understanding-the-vtable-entries)上举了一个例子说明`offset to top`的作用：

> Let's say you have a derived class D that derives from a base class, B1. What happens when you try to cast a D instance to type B1? Since functions that take a B1 object don't know anything about D, part of the D vtable must also be a valid B1 vtable. This is easy enough - just make the start of the D vtable look like a B1 vtable, and add on any additional entries we need after that. Functions expecting a B1 will be happy, because they won't use any part of the vtable beyond what they're expecting for a B1.

> However, what happens if D now also derives from B2? The pointer to the D vtable can't be both a valid B1 vtable and a valid B2 vtable! The compiler solves this by appending a separate B2 vtable to the end of our combined D/B1 vtable, and adjusts the vtable-pointer manually when we try to cast from a D to a B2.

> However, this leads to a new problem - what happens when we try to cast back from a B2 to a D? The compiler can't just adjust the vtable-pointer backwards by the same amount it adjusted the pointer previously, because it doesn't actually know for sure that the B2 object we're giving it is of type D! In particular, dynamic_cast<D>() must be able to tell if our object is or isn't of type D. For that, it needs to access the object's RTTI, and for that, it needs to know where the start of the original object's vtable is. This is the purpose of the offset-to-top value - it gives us the offset to the start of the original object's vtable, we get our object's RTTI, and the vengeful god of C++ allows our crops to grow for another season.

`dynamic_cast`的原理是：

> If, in the most derived object pointed (referred) to by v, v points (refers) to a public base class subobject of a T object [note: this can be checked at compile time], and if only one object of type T is derived from the subobject pointed (referred) to by v, the result is a pointer (an lvalue referring) to that T object. 

> Otherwise, if v points (refers) to a public base class subobject of the most derived object, and the type of the most derived object has an unambiguous public base class of type T, the result is a pointer (an lvalue referring) to the T subobject of the most derived object. 

> Otherwise, the run-time check fails. 

`dynamic_cast`的算法有点复杂，见[ABI](http://mentorembedded.github.io/cxx-abi/abi.html#rtti)：
{% highlight c %}
extern "C" 
void* __dynamic_cast ( const void *sub,
 		  const abi::__class_type_info *src,
 		  const abi::__class_type_info *dst,
 		  std::ptrdiff_t src2dst_offset);
/* sub: source address to be adjusted; nonnull, and since the
 *      source object is polymorphic, *(void**)sub is a virtual
 pointer.
 * src: static type of the source object.
 * dst: destination type (the "T" in "dynamic_cast<T>(v)").
 * src2dst_offset: a static hint about the location of the
 *    source subobject with respect to the complete object;
 *    special negative values are:
 *       -1: no hint
 *       -2: src is not a public base of dst
 *       -3: src is a multiple public base type but never a
 *           virtual base type
 *    otherwise, the src type is a unique public nonvirtual
 *    base type of dst at offset src2dst_offset from the
 *    origin of dst.
 */
{% endhighlight %}