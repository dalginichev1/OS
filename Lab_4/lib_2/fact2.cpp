#include "Derivative_2.hpp"
#include "QuickSort.hpp"
#include "AbsDerivative.hpp"
#include "AbsSort.hpp"

extern "C"
{
    __attribute__((visibility("default"))) AbsDerivative * CreateDerivative()
    {
        return new Derivative2();
    }

    __attribute__((visibility("default"))) AbsSort * CreateSort()
    {
        return new QuickSort();
    }

    __attribute__((visibility("default"))) void DeleteDerivative(AbsDerivative * p)
    {
        delete p;
    }

    __attribute__((visibility("default"))) void DeleteSort(AbsSort * p)
    {
        delete p;
    }
}