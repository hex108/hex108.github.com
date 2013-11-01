---
layout: post
title: 向GDB提交Patch的流程简介
category: GDB
summary: 提交的主要包括Patch和Test Case。
tags: [GDB, Test Case]
---

{{ page.title }}
================

{{ page.summary }}

## 提交Patch

## 编写GDB测试用例简介
如果需要向GDB提交一个Patch或BUG，一个编写完好的Test Case将会是一个加分项（使Patch更快被接受，BUG更快地得到解决）。

### 编写测试用例
找到相应文件，以此为基础进行修改，OK！

### 运行测试
make check RUNTESTFLAGS='const-var.exp'