---
layout: note
title: 算法
---

{{ page.title }}
================

### 算法复杂度

### 抽象的算法

* [贪心法(Greedy algorithm)](http://en.wikipedia.org/wiki/Greedy_algorithm)和[动态规化(Dynamic programing)](http://en.wikipedia.org/wiki/Dynamic_programming)

  我以前写过一篇[文章](http://www.cppblog.com/hex108/archive/2011/08/24/154243.html)比较了这两种方法。再次看了wiki上的解释后，我总结了以下几点：

  适合用贪心法和动态规化解决的问题一般都有[Optimal substructure](http://en.wikipedia.org/wiki/Optimal_substructure)。
  > A problem exhibits optimal substructure if an optimal solution to the problem contains optimal solutions to the sub-problems.

  适合贪心法动态规化的条件有所不同。
  > Typically, a greedy algorithm is used to solve a problem with optimal substructure if it can be proved by induction that this is optimal at each step. 

  > Otherwise, providing the problem exhibits [overlapping subproblems](http://en.wikipedia.org/wiki/Overlapping_subproblem) as well, dynamic programming is used. 

  > If there are no appropriate greedy algorithms and the problem fails to exhibit overlapping subproblems, often a lengthy but straightforward search of the solution space is the best alternative.

  贪心法能解决的问题动态规化都能解决，不过贪心法更快。

  > CLRS 16.2 Nevertheless, beneath every greedy algorithm, ther is almost always a more cumbersome dynamic-progrmming solution.
  
  > For many optimization problems, using dynamic programming to determine the best choices is overkill; Simpler, more effiiect algorithms will do(eg: greedy algorithm).

### 具体的算法
* KMP, Boyer-Moore
  
  推荐文章：[字符串匹配的KMP算法](http://www.ruanyifeng.com/blog/2013/05/Knuth%E2%80%93Morris%E2%80%93Pratt_algorithm.html)，[字符串匹配的Boyer-Moore算法](http://www.ruanyifeng.com/blog/2013/05/boyer-moore_string_search_algorithm.html)。

* A*

  A*算法在宽度优先搜索的基础上增加了“代价”的概念，这样每次能优先搜索相对更优的结点。

  推荐文章：[A* Pathfinding for Beginners](http://www.policyalmanac.org/games/aStarTutorial.htm)，该文章最后附有代码。

### 数据结构

* 二叉树

  对树的遍历可以有多种实现方式：递归，栈，**Morris遍历**。前两种实现方式比较常见，容易理解，理解Morris遍历的关键在于理解[Threaded Binary Tree](http://en.wikipedia.org/wiki/Threaded_binary_tree)。以中序遍历为例，Morris遍历的代码为[morris_visit](code/morris_visit_tree.txt)。
 
### 技巧
* 缓存(cache)

  在动态规化的备忘录方法中得到了很好的使用。  

* Hash

* 二分

* 剪枝 

* 预处理

  e.g. 处理数组前先对它排序。

* Dummy Node
