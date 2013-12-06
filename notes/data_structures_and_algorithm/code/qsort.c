#include <stdio.h>
#include <stdlib.h>

void swap(int a[], int i, int j){
    int tmp = a[i];
    a[i] = a[j];
    a[j] = tmp;
}

void q_sort(int a[], int start, int end){
    if(start >= end)
        return;

    // choose random index for pivot
    int random_index = (int)random() % (end - start + 1) + start;
    //printf("%d ~ %d, random : %d(%d)\n", start, end, random_index, a[random_index]);
    swap(a, start, random_index);

    // partion 
    int pivot = a[start];
    int left = start, right = end + 1;
    int i;
    // partion the array to three parts: < pivot | = pivot | > pivot
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

// test
void print_arr(int a[], int n){
    int i;
    for(i = 0; i < n; i ++)
        printf("%d, ", a[i]);

    printf("\n");
}

int main()
{
    int a[] = {10, 50, 2, 1, 2, 8, 9, 30, 70, 8, 8};
    int n = sizeof(a) / sizeof(a[0]);

    quick_sort(a, n);

    print_arr(a, n);

    return 0;
}
