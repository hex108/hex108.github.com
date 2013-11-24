---
layout: note 
title: 迭代器的实现细节
---

{{ page.title }}
================

看了一下vector和list里的迭代器实现，才发现它不是像《Head First设计模式》里说的迭代器模式。vector和list提供的迭代器都是实现在各自的类里的，提供的接口都是相同的，所以给用户的感觉就是它们的迭代器都是继承于同一接口。从用户使用的角度来说它们达到了“迭代器模式”的效果。为什么没有继承于同一接口呢？我猜测是因为：对于迭代器的各个操作vector和list都得实现一遍，而C++的库本身已经很复杂了，就没必要再增加一个接口了。

以vector为例说明迭代器的实现细节。

迭代器在vector中的定义如下，其实iterator就是一个指针！
{% highlight c++ %}
template <class _Tp, class _Alloc = __STL_DEFAULT_ALLOCATOR(_Tp) >
class vector : protected _Vector_base<_Tp, _Alloc> 
{
public:
  typedef _Tp value_type;
  typedef value_type* iterator;
  typedef const value_type* const_iterator;
  ...
};
{% endhighlight %}

当知道iterator其实是一个指针以后，下面的代码就比较好理解了。
{% highlight c++ %}
for(vector<int>::iterator iter = v.begin(); iter != v.end(); iter ++)
{% endhighlight %}

如果声明vector对象时不指定内存分配器_Alloc，vector的空间一般由alloc直接分配，形成一个数组，v.begin()返回的就是数组的首地址，v.end()返回的是数组最后一个地址的下一个地址。
{% highlight c++ %}
iterator begin() { return _M_start; }
iterator end() { return _M_finish; }
{% endhighlight %}

有兴趣的可以想一下怎么实现rbegin()和rend()以实现下面的功能。
{% highlight c++ %}
for(vector<int>::iterator iter = v.rbegin(); iter != v.rend(); iter ++)
    cout << *iter << endl;
{% endhighlight %}