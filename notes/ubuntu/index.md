---
layout: note
title: Ubuntu
---

{{ page.title }}
================

* [Ubuntu 屏幕闪烁并且移位问题](http://www.linuxidc.com/Linux/2012-10/71773.htm)
  > 屏幕闪烁移位，其实是显卡驱动的问题，N卡官网给的驱动不好用，解决方法： 
 
  > sudo add-apt-repository ppa:bumblebee/stable

  > sudo apt-get update

  > sudo apt-get install bumblebee bumblebee-nvidia

  > sudo reboot

  > 解决闪烁问题：

  > 终端执行：`xinput list`，你会发现存在两个Video Bus，记住第二个的ID，我的第二个Vedio Bus的ID是8，然后终端执行:`xinput set-prop 8 "Device Enabled" 0`，但是这样重启计算机后会失效，因此可以将该指令添加到.bashrc下，这样登录时会执行该指令，至此，频繁而且不定时的闪屏问题不见了。