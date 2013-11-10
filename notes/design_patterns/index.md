---
layout: note
title: 设计模式
---

<<[Head First 设计模式](http://book.douban.com/subject/2243615/)>>写得真好！

### 常用的设计模式

#### 创建型模式

* 单例模式(Singleton)
  
  确保程序中一个类最多只有一个实例，提供访问这个实例的全局点。代码片段为[singleton](code/singleton.txt)。

  推荐文章：[C++ Singleton (单例) 模式最优实现](http://blog.yangyubo.com/2009/06/04/best-cpp-singleton-pattern/)

* 工厂模式(Factory)：工厂方法模式(Factory Method)，抽象工厂模式(Abstract Factory)

  它帮助我们针对抽象编程，而不要针对具体类编程。

  工厂方法模式定义了一个创建对象的接口，但由子类决定要实例化的类是哪一个。工厂方法让类把实例化推迟到子类。代码片段为[factory_method](code/factory_method.txt)。

  抽象工厂模式提供一个接口，用于创建相关或依赖对象的家族，而不需要明确指定具体类。代码片段为[abstract_factory](code/abstract_factory.txt)。

  **OO原则**：针对接口编程，不针对实现编程；为交互对象的松耦合设计而努力；类应该对扩展开放，对修改关闭；依赖抽象，不要依赖具体类。
  
#### 行为型模式

* 策略模式(Strategy)

  定义了算法族，分别封装起来，让它们之间可以互相替换，此模式让算法的变化独立于使用算法的客户。代码片段为[strategy](code/strategy.txt)。

  **OO原则**：封装变化；多用组合，少用继承；针对接口编程，不针对实现编程。