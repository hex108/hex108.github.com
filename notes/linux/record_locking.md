---
layout: note 
title: 记录锁
---

{{ page.title }}
================

记录锁的功能是：当一个进程正在读或修改文件的某个部分时，它可以阻止其他进程修改同一文件区。函数`fcntl`支持记录锁功能，不过`fcntl`默认支持的是建议性记录锁。如果想让它支持强制性记录锁，需要保证如下2点（引自[Linux 2.6 中的文件锁](http://www.ibm.com/developerworks/cn/linux/l-cn-filelock/)）：

> 1. 使用“-o mand”选项来挂载文件系统
> 2. 修改要加强制锁的文件的权限：设置 SGID 位，并清除组可执行位。这种组合通常来说是毫无意义的，系统用来表示该文件被加了强制锁

示例代码可见[fcntl记录锁实例](http://www.cnblogs.com/riskyer/archive/2013/08/01/3230783.html)。

## 推荐阅读
* [2 Types of Linux File Locking (Advisory, Mandatory Lock Examples)](http://www.thegeekstuff.com/2012/04/linux-file-locking-types/)
* [Linux 2.6 中的文件锁](http://www.ibm.com/developerworks/cn/linux/l-cn-filelock/)