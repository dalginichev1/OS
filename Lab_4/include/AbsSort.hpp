#pragma once

class AbsSort
{
    public:
        virtual int * Sort(int * array, int size) = 0;
        virtual ~AbsSort() = default;
};