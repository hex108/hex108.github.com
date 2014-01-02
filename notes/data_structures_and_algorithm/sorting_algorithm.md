---
layout: note
title: 排序算法
---

{{ page.title }}
================

## 1. 内部排序算法
Wiki上对[排序算法](http://en.wikipedia.org/wiki/Sorting_algorithm)有一个很好的总结。稳定排序有：冒泡排序、插入排序、归并排序，不稳定排序有：快速排序、堆排序、选择排序。

* [快速排序](qsort.html)
* [冒泡排序](bubble_sort.html)
* [堆排序](http://en.wikipedia.org/wiki/Heapsort)
* [插入排序](http://en.wikipedia.org/wiki/Insertion_sort)
* [选择排序](http://en.wikipedia.org/wiki/Selection_sort)
* [归并排序](http://en.wikipedia.org/wiki/Merge_sort)


## 2. 外部排序算法
[External sorting](http://en.wikipedia.org/wiki/External_sorting)上举了一个算法例子：
> 1. 将大文件分为K个块，分别用内部排序（如: quicksort）进行排序，然后将它们写入各个临时文件。
> 2. 用K路归并排序对这K个块进行归并。

第1步可以采用[置换选择算法](http://www.cs.bilkent.edu.tr/~canf/CS351Fall2009/cs351lecturenotes/week4/index.html)（[置换选择排序与选择排序的区别](http://stackoverflow.com/questions/16326689/replacement-selection-sort-v-selection-sort)），这样在同样的内存下对更多的数据进行排序，每个临时文件就能分到更多的数据，K路归并排序时每路能预读的数据更多了（因为K变小了）。置换选择算法还有一个变体：用堆排序取代选择排序，这样时间复杂度更低，详情见[选择置换+败者树搞定外部排序](http://www.cnblogs.com/benjamin-t/p/3325401.html)。