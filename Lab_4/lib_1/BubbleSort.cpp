#include "BubbleSort.hpp"

#include <algorithm>

int * BubbleSort::Sort(int * array, int size)
{
    int i = 0;
    int j = 0;

    for (i = 0; i < size - 1; ++i)
    {
        for (j = 0; j < size - i - 1; ++j)
        {
            if (array[j] > array[j + 1])
            {
                std::swap(array[j], array[j + 1]);
            }
        }
    }

    return array;
}