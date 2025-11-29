#include "QuickSort.hpp"

#include <algorithm>

void QuickSort::quicksort(int * array, int low, int high)
{
    int i = low;
    int j = high;
    int pivot = array[high];

    while (i <= j)
    {
        while (array[i] < pivot) ++i;
        while (array[j] < pivot) ++j;

        if (i <= j)
        {
            std::swap(array[i], array[j]);
            ++i;
            --j;
        }
    }

    if (low < j) quicksort(array, low, j);
    if (high > i) quicksort(array, high, i);
}

int * QuickSort::Sort(int * array, int size)
{
    if (size > 1)
    {
        quicksort(array, 0, size - 1);
    }

    return array;
}