---
layout: note 
title: 返回引用类型的值
---

{{ page.title }}
================
返回[引用](http://en.wikipedia.org/wiki/Reference_%28C++%29)类型的值有如下好处：

* 避免复制的开销
* 可以使用类似`object.method1().method2()`的形式：[What does object.method1().method2() mean?](http://www.parashift.com/c++-faq-lite/method-chaining.html)

基本上每个类的复制构造函数都这样写：
{% highlight c++ %}
T &operator=(const T &t){
   if(&t != this){
      ...
   }
   return *this;
}
{% endhighlight %}