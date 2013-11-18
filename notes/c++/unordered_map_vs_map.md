---
layout: note 
title: unordered_map VS map
---

{{ page.title }}
================

首先分别看看[unordered_map](http://en.cppreference.com/w/cpp/container/unordered_map)和[map](http://en.cppreference.com/w/cpp/container/map)的描述：

> **Unordered map** is an **associative container** that contains key-value pairs with unique keys. Search, insertion, and removal of elements have average constant-time complexity.
Internally, the elements are not sorted in any particular order, but organized into **buckets**. Which bucket an element is placed into depends entirely on the hash of its key. This allows fast access to individual elements, since once hash is computed, it refers to the exact bucket the element is placed into.

> **std::map** is a **sorted associative container** that contains key-value pairs with unique keys. Keys are sorted by using the comparison function Compare. Search, removal, and insertion operations have logarithmic complexity. Maps are usually implemented as **red-black trees**.

对于一般的使用而言，unordered map就够用了，而且更高效，它的时间复杂度为O(1)，map的时间复杂度为O(lgN)。那什么时候使用map更合适呢？Stackoverflow上关于这个问题有一个[讨论](http://stackoverflow.com/questions/2196995/is-there-any-advantage-of-using-map-over-unordered-map-in-case-of-trivial-keys)，主要有以下几个观点：

* map keep their elements ordered. If you can't give up that, obviously you can't use an unordered_map.

* unordered_map generally uses more memory. A map just has a few house-keeping pointers then memory for each object. Contrarily, unordered_map's have a big array (these can get quite big in some implementations) and then additional memory for each object. If you need to be memory-aware, a map should prove better, because it lacks the large array. So, if you need pure lookup-retrieval, I'd say an unordered_map is the way to go. But there are always trade-offs, and if you can't afford them, then you can't use it.

* Just from personal experience, I found an enormous improvement in performance (measured, of course) when using an unordered_map isntead of a map in a main entity look-up table. On the other hand, I found it was much slower at repeatedly inserting and removing elements. It's great for a relatively static collection of elements, but if you're doing tons of insertions and deletions the hashing + bucketing seems to add up. (Note, this was over many iterations.)

  我觉得unodered性能不好的原因可能是buckets增大或减小因而需要重新hash所有的key而导致的。

### 后序

在C++11以前unordered_map在STL里的对应容器是hash_map，不过hash_map不在C++标准库里。hash_map发展的[历史](http://en.wikipedia.org/wiki/Hash_map_%28C++%29#History)为：

> The first widely used implementation of hash tables in the C++ language was hash_map, hash_set, hash_multimap, hash_multiset class templates of the SGI STL. Due to its usefulness it was later included in several other implementations of the C++ standard library, e.g. in the GCC's libstdc++ or in MSVC standard library.

> The hash_* class templates were proposed into C++ TR1 and were accepted under names unordered_*. Later, they were incorporated into the C++11 revision of the C++ standard. An implementation is also available in the Boost C++ Libraries as <boost/unordered_map.hpp>.

所以hash_map没有被包括进C++标准库里的原因在于那时候还没有hash_map，它是后来发展起来的。