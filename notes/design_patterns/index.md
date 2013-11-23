---
layout: note
title: 设计模式
---

{{ page.title }}
================

<<[Head First 设计模式](http://book.douban.com/subject/2243615/)>>写得真好！通过这些设计模式学到不少OO思想。

## 1. 常用的设计模式

设计模式可分为三大类：[创建型模式](#CreatinalPattern)，[结构型模式](#StructuralPattern)，[行为型模式](#BehavioralPattern)，详见[Wiki](http://zh.wikipedia.org/zh-cn/%E8%AE%BE%E8%AE%A1%E6%A8%A1%E5%BC%8F_%28%E8%AE%A1%E7%AE%97%E6%9C%BA%29)。

<h3 id="CreatinalPattern">创建型模式</h3>

* 单例模式(Singleton)
  
  确保程序中一个类最多只有一个实例，提供访问这个实例的全局点。代码片段为[singleton](code/singleton.txt)。

  推荐文章：[C++ Singleton (单例) 模式最优实现](http://blog.yangyubo.com/2009/06/04/best-cpp-singleton-pattern/)

* 工厂模式(Factory)：工厂方法模式(Factory Method)，抽象工厂模式(Abstract Factory)：由子类决定实例化哪个具体类

  它帮助我们针对抽象编程，而不要针对具体类编程。

  工厂方法模式定义了一个创建对象的接口，但由子类决定要实例化的类是哪一个。工厂方法让类把实例化推迟到子类。代码片段为[factory_method](code/factory_method.txt)。

  抽象工厂模式提供一个接口，用于创建相关或依赖对象的家族，而不需要明确指定具体类。代码片段为[abstract_factory](code/abstract_factory.txt)。
  
<h3 id="StructuralPattern">结构型模式</h3>

* 装饰者模式(Decorator)：不改变接口，但加入责任

  动态地将责任附加到对象上。若要**扩展功能**，装饰者提供了比继承更有弹性的替代方案。装饰者可以在所委托被装饰者的行为之前或之后，加上自己的行为， 以达到特定目的。代码片段为：[decorator](code/decorator.txt)。

  真实世界的装饰者：Java IO，以下例子来源于[stackoverflow](http://stackoverflow.com/questions/6366385/decorator-pattern-for-io)：
  
  > First open an inputstream of it:

  > FileInputStream fis = new FileInputStream("/objects.gz");

  > We want speeeed, so let's buffer it in memory:

  > BufferedInputStream bis = new BufferedInputStream(fis);

  > The file is gzipped, so we need to ungzip it:

  > GzipInputStream gis = new GzipInputStream(bis);

  当初用Java写程序的时候确实是这样！不过今天才搞懂原因。:( 装饰者就是这样的，虽然灵活，会导致设计中出现许多小对象，如果过度使用，会让程序变得很复杂。

* 适配器模式(Adapter)：将一个接口转换成另一个接口

  将一个类的接口，转换成客户期望的另一个接口。适配器让原本接口不兼容的类可以合作无间。适配器分为对象适配器和类适配器。对象适配器使用组合的方式实现，不仅可以适配某个类，还可以适配该类的任何一个子类。类适配器使用继承的方式实现，不需要重新实现事个被适配者。代码片段为：[adapter](code/adapter.txt)。
 
  顿时觉得C++里的stack, queue和priority_queue亲切和熟悉了很多！现在一眼就能看懂STL里stack的实现了，深藏功与名！stack里的默认容器是deque而不是vector原因详见[vectors and deques](http://www.gotw.ca/publications/mill10.htm)。
  
      template<typename T, typename C = deque<T>>
      class stack { // §iso.23.6.5.2
          ...
      protected:
          C c;
      };

* 外观模式(Facade)：让接口更简单
  
  提供了一个统一的接口，用来访问子系统中的一群接口。外观定义了一个高层接口，让子系统更容易使用。代码片段为：[facade](code/facade.txt)。

* 组合模式(Composite)：客户可以将对象的集合以及个别的对象一视同仁

  允许你将对象组合成树形结构来表现“整体/部分”层次结构。组合能让客户以一致辞的方式处理个别对象以及对象组合。代码片段见[composite](code/composite.txt)。  

<h3 id="BehavioralPattern">行为型模式</h3>

* 策略模式(Strategy)：封装可互换的行为，然后使用委托来决定要采用哪一个行为

  定义了算法族，分别封装起来，让它们之间可以互相替换，此模式让算法的变化独立于使用算法的客户。代码片段为[strategy](code/strategy.txt)。

* 观察者模式(Observer)：当某个状态改变时，允许一群对象能被通知到

  定义了对象之间的一对多依赖，这样一来，当一个对象改变状态时，它的所有依赖都会收到通知并自动更新。代码片段为[observer](code/observer.txt)。

  学完这个模式，觉得MVC也了解了！

  下面的代码用设计模式来理解，瞬间觉得高档了很多！
  > JButton button = new JButton("test");

  > button.addActionListener(new Action1());

  > button.addActionListener(new Action2()); 

* 命令模式(Command)

  将“请求”封装成对象，以便使用不同的请求、队列或者日志来参数化其他对象。命令模式也支持可撤销的操作。代码片段为[command](code/command.txt)。

  应用：线程池、工作队列。

* 模板方法模式(Template method)：子类决定如何实现算法中的某些步骤

  在一个方法中定义一个算法的骨架，而将一些步骤延迟到子类中。模板方法使得子类可以在不改变算法的情况下，重新定义算法中的某些步骤。代码片段见[template_method](code/template_method.txt)。

  **工厂方法是模板方法的一种特殊版本。**

  C++ STL中的Algorithm都是模板方法。

* 迭代器模式(Iterator)：提供一个方式来遍历集合，而无须暴露集合的实现

  提供一种方法顺序访问一个聚合对象中的各个元素，而又不暴露其内部的表示。代码片段见[iterator](code/iterator.txt)。

  C++ STL里的iterator是这个模式。

* 状态模式(State)
  
  允许对象在内部状态改变时改变它的行为，对象看起来好像修改了它的类。代码片段见[state](code/state.txt)。

  通过类而不是很多if语句实现状态机。增加一个状态时，状态模式增加避免了修改if语句，但是状态类之间产生了依赖。

## 2. 设计模式里的OO思想

  * 类应该对扩展开放，对修改关闭

  * 多用组合，少用继承
  
    通过动态地组合对象，可以写新的代码添加新的功能，而无须修改现有代码。既然没有改变现有代码，那么引进bug或产生意外副作用的机会将大幅度减少。

  * 针对接口编程，不针对实现编程
 
    依赖抽象，不要依赖具体类。

  * 封装变化

    例子：策略模式。

  * 松耦合

  * “最小知识”原则（墨忒耳法则）：只和你的密友谈话。
    
    这个原则希望我们在设计中，不要让太多的类耦合在一起，免得修改系统中一部分，会影响到其他部分。就任何对象而言，在该对象方法内，我们只应该调用属于以下范围的方法：
    
    * 该对象本身
    
    * 被当做方法的参数传进来的对象

    * 此方法创建或实例化的任何对象

    * 对象的任何组件（对象的数据）
  
  * 好莱坞原则

    别调用我们，我们会调用你。在该原则之下，我们允许低层组件将自己挂钩到系统上，但是高层组件会决定什么时候和怎样使用这些低层组件。这样能够防止高层组件依赖低层组件，而同时低层组件又依赖高层组件。

  * 单一责任

    一个类应该只有一个引起变化的原因。尽量让每个类保持单一责任，只做一件事。比如：不要让vector既提供集合的功能，又提供迭代器的功能。
