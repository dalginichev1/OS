#pragma once

#include "AbsSort.hpp"

class QuickSort: public AbsSort
{
    public:
        int * Sort(int * array, int size) override;
        ~QuickSort() override = default;
    private:
        void quicksort(int * array, int low, int high);
};