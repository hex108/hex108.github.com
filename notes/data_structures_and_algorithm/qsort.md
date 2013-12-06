---
layout: note
title: 快速排序
---

{{ page.title }}
================

按照《编程珠玑》上的描述把qsort5写好了，代码[qsort.c](code/qsort.c)如下：
{% highlight c %}
void swap(int a[], int i, int j){
    int tmp = a[i];
    a[i] = a[j];
    a[j] = tmp;
}

void q_sort(int a[], int start, int end){
    if(start >= end)
        return;

    // 随机选择一个index作为哨兵
    int random_index = (int)random() % (end - start + 1) + start;
    //printf("%d ~ %d, random : %d(%d)\n", start, end, random_index, a[random_index]);
    swap(a, start, random_index);

    // partion 
    int pivot = a[start];
    int left = start, right = end + 1;
    int i;
    // partion the array to three parts:
    //      [start + 1 ... left ] < pivot
    //      [left + 1 ... right - 1] = pivot
    //      [right ... end] > pivot
    for(i = start + 1; i < right;){
        if(a[i] < pivot){
            swap(a, ++left, i);
            i++;
        }else if(a[i] > pivot)
            swap(a, --right, i);
        else
            i ++;
    }
    swap(a, left, start);

    q_sort(a, start, left - 1);
    q_sort(a, right, end);
}

void quick_sort(int a[], int n){
   q_sort(a, 0, n - 1);
}
{% endhighlight %}