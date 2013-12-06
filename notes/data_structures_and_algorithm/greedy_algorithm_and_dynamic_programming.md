---
layout: note
title: 贪心算法与动态规化
---

{{ page.title }}
================

本文主要比较[贪心法(Greedy algorithm)](http://en.wikipedia.org/wiki/Greedy_algorithm)和[动态规化(Dynamic programing)](http://en.wikipedia.org/wiki/Dynamic_programming)的异同。我以前写过一篇[文章](http://www.cppblog.com/hex108/archive/2011/08/24/154243.html)比较了这两种方法。再次看了wiki上的解释后，我总结了以下几点：

* 适合用贪心法和动态规化解决的问题一般都有[Optimal substructure](http://en.wikipedia.org/wiki/Optimal_substructure)。
> A problem exhibits optimal substructure if an optimal solution to the problem contains optimal solutions to the sub-problems.

* 适合贪心法动态规化的条件有所不同。
  > Typically, a greedy algorithm is used to solve a problem with optimal substructure if it can be proved by induction that this is optimal at each step. 

  > Otherwise, providing the problem exhibits [overlapping subproblems](http://en.wikipedia.org/wiki/Overlapping_subproblem) as well, dynamic programming is used. 

  > If there are no appropriate greedy algorithms and the problem fails to exhibit overlapping subproblems, often a lengthy but straightforward search of the solution space is the best alternative.

* 贪心法能解决的问题动态规化都能解决，不过贪心法更快。

  > CLRS 16.2 Nevertheless, beneath every greedy algorithm, ther is almost always a more cumbersome dynamic-progrmming solution.
  
  > For many optimization problems, using dynamic programming to determine the best choices is overkill; Simpler, more effiiect algorithms will do(eg: greedy algorithm).
