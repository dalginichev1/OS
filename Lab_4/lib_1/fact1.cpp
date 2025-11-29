#include "Derivative_1.hpp"
#include "BubbleSort.hpp"
#include "AbsDerivative.hpp"
#include "AbsSort.hpp"

extern "C"
{
    __attribute__((visibility("default"))) AbsDerivative * CreateDerivative()
    {
        return new Derivative1();
    }

    __attribute__((visibility("default"))) AbsSort * CreateSort()
    {
        return new BubbleSort();
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