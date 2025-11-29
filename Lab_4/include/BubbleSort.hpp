#pragma once

#include "AbsSort.hpp"

class BubbleSort: public AbsSort
{
    int * Sort(int * array, int size) override;
    ~BubbleSort() override = default;
};