---
layout: note
title: 算法
---

### 算法复杂度

### 一些思想
* 缓存(cache)

* Hash

* 二分

* 剪枝 

* 预处理

  e.g. 处理数组前先对它排序。

### 一些算法

* KMP, Boyer-Moore
  
  推荐文章：[字符串匹配的KMP算法](http://www.ruanyifeng.com/blog/2013/05/Knuth%E2%80%93Morris%E2%80%93Pratt_algorithm.html)，[字符串匹配的Boyer-Moore算法](http://www.ruanyifeng.com/blog/2013/05/boyer-moore_string_search_algorithm.html)。

* A*

  A*算法在宽度优先搜索的基础上增加了“代价”的概念，这样每次能优先搜索相对更优的结点。

  推荐文章：[A* Pathfinding for Beginners](http://www.policyalmanac.org/games/aStarTutorial.htm)，该文章最后附有代码。

### 一些数据结构

* 二叉树

  对树的遍历可以有多种实现方式：递归，栈，**Morris遍历**。前两种实现方式比较常见，容易理解，理解Morris遍历的关键在于理解[Threaded Binary Tree](http://en.wikipedia.org/wiki/Threaded_binary_tree)。以中序遍历为例，Morris遍历的代码为[morris_visit](code/morris_visit_tree.txt)。
 
### 一些技巧
* Dummy Node
