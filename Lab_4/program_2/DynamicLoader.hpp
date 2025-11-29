#pragma once

#include "AbsDerivative.hpp"
#include "AbsSort.hpp"

#include <string>

class DynamicLoader
{
    public:
        using CreateDerivativeFunc = AbsDerivative*(*)();
        using CreateSortFunc = AbsSort*(*)();
        using DeleteDerivativeFunc = void(*)(AbsDerivative*);
        using DeleteSortFunc = void(*)(AbsSort*);

        DynamicLoader();
        ~DynamicLoader();

        bool Load(const std::string& path);
        std::string Error() const;

        AbsDerivative* CreateDerivative();
        AbsSort* CreateSort();
        void DeleteDerivative(AbsDerivative*);
        void DeleteSort(AbsSort*);
    
    private:
        void *handle_;
        std::string lastErr;

        CreateDerivativeFunc createDer;
        CreateSortFunc createSort;
        DeleteDerivativeFunc deleteDer;
        DeleteSortFunc deleteSort;
};