---
layout: note
title: 设计模式
---

{{ page.title }}
================

<<[Head First 设计模式](http://book.douban.com/subject/2243615/)>>写得真好！通过这些设计模式学到不少OO思想。

## 1. 常用的设计模式

### 创建型模式

* 单例模式(Singleton)
  
  确保程序中一个类最多只有一个实例，提供访问这个实例的全局点。代码片段为[singleton](code/singleton.txt)。

  推荐文章：[C++ Singleton (单例) 模式最优实现](http://blog.yangyubo.com/2009/06/04/best-cpp-singleton-pattern/)

* 工厂模式(Factory)：工厂方法模式(Factory Method)，抽象工厂模式(Abstract Factory)

  它帮助我们针对抽象编程，而不要针对具体类编程。

  工厂方法模式定义了一个创建对象的接口，但由子类决定要实例化的类是哪一个。工厂方法让类把实例化推迟到子类。代码片段为[factory_method](code/factory_method.txt)。

  抽象工厂模式提供一个接口，用于创建相关或依赖对象的家族，而不需要明确指定具体类。代码片段为[abstract_factory](code/abstract_factory.txt)。
  
### 结构型模式

* 装饰者模式(Decorator)

  动态地将责任附加到对象上。若要**扩展功能**，装饰者提供了比继承更有弹性的替代方案。装饰者可以在所委托被装饰者的行为之前或之后，加上自己的行为， 以达到特定目的。代码片段为：[decorator](code/decorator.txt)。

  真实世界的装饰者：Java IO，以下例子来源于[stackoverflow](http://stackoverflow.com/questions/6366385/decorator-pattern-for-io)：
  
  > First open an inputstream of it:

  > FileInputStream fis = new FileInputStream("/objects.gz");

  > We want speeeed, so let's buffer it in memory:

  > BufferedInputStream bis = new BufferedInputStream(fis);

  > The file is gzipped, so we need to ungzip it:

  > GzipInputStream gis = new GzipInputStream(bis);

  当初用Java写程序的时候确实是这样！不过今天才搞懂原因。:( 装饰者就是这样的，虽然灵活，会导致设计中出现许多小对象，如果过度使用，会让程序变得很复杂。

### 行为型模式

* 策略模式(Strategy)

  定义了算法族，分别封装起来，让它们之间可以互相替换，此模式让算法的变化独立于使用算法的客户。代码片段为[strategy](code/strategy.txt)。

## 2. 设计模式里的OO思想

  * 类应该对扩展开放，对修改关闭

  * 多用组合，少用继承
  
    通过动态地组合对象，可以写新的代码添加新的功能，而无须修改现有代码。既然没有改变现有代码，那么引进bug或产生意外副作用的机会将大幅度减少。

  * 针对接口编程，不针对实现编程
 
    依赖抽象，不要依赖具体类。

  * 封装变化

    例子：策略模式。