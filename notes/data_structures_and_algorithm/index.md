---
layout: note
title: 数据结构与算法
---

{{ page.title }}
================

## 1. 数据结构
有的问题数据结构选对了，接下来的就简单了。

* 二叉树

  对树的遍历可以有多种实现方式：递归，栈，**Morris遍历**。前两种实现方式比较常见，容易理解，理解Morris遍历的关键在于理解[Threaded Binary Tree](http://en.wikipedia.org/wiki/Threaded_binary_tree)。以中序遍历为例，Morris遍历的代码为[morris_visit](code/morris_visit_tree.txt)。

## 2. 算法
### 算法复杂度
我最满意[《算法引论——一种创造性方法》](http://book.douban.com/subject/1436134/)上对它的讲解：
> 给定一个问题和规模的定义，我们的目标是找到一个与规模有关的算法运行时间的表达式。**最通常的选择是最坏的输入。**为什么不用最佳输入或者平均输入呢？因为大多数情况并不具代表性，最佳输入通常会被排除。平均输入是一个好的选择，但它有时很难被有效评估。目前还没有全面、易用的平均状况分析技术。因此我们仅可对少数问题进行平均性能分析，绝大部分仍要凭借最坏状况分析。选择最坏输入作为指标是非常有益的。有些时候，从经验上说最坏输入非常接近于平均输入。另一方向，即使最坏输入完全不同于平均输入，那些在最坏输入情况下具有最佳性能的算法对于其他输入情况来说也有很好的性能。

### 算法几大类
* [排序](sorting_algorithm.html)
* [搜索](search_algorithm.html)
* [字符串匹配](string_match.html)
* [贪心算法与动态规化](greedy_algorithm_and_dynamic_programming.html)

### 技巧
缓存(在动态规化的备忘录方法中得到了很好的使用）、Hash、二分、剪枝、预处理（如：处理数组前先对它排序）、Dummy Node。