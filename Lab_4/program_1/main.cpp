#include <iostream>
#include "../include/AbsDerivative.hpp"
#include "../include/AbsSort.hpp"

extern "C"
{
    AbsDerivative * CreateDerivative();
    AbsSort * CreateSort();
    void DeleteDerivative(AbsDerivative*);
    void DeleteSort(AbsSort*);
}

int main(int argc, char * argv[])
{
    AbsDerivative * der = CreateDerivative();
    AbsSort * sort = CreateSort();

    while (true)
    {
        int cmd = 0;
        std::cout << "Введите, что хотите сделать" << std::endl;
        std::cout << "0 Конец программы" << std::endl;
        std::cout << "1 Производная" << std::endl;
        std::cout << "2 Сортировка" << std::endl;
        std::cin >> cmd;

        if (cmd == 1)
        {
            float a;
            float dx;
            std::cout << "Введите точку" << std::endl;
            std::cin >> a;
            std::cout << "Введите отклонение" << std::endl;
            std::cin >> dx;
            std::cout << der->Derivative(a, dx) << std::endl;
        }

        else if (cmd == 2)
        {
            int n;
            std::cout << "Введите размер массива" << std::endl;
            std::cin >> n;
            int * array = new int[n];

            size_t i = 0;
            std::cout << "Введите элементы массива" << std::endl;
            for (i = 0; i < n; ++i) std::cin >> array[i];

            sort->Sort(array, n);

            for (i = 0; i < n; ++i) std::cout << array[i] << " ";
            std::cout << std::endl;

            delete[] array;
        }

        else if (cmd == 0) break;
    }
    DeleteDerivative(der);
    DeleteSort(sort);

    return 0;
}