#include <iostream>
#include "DynamicLoader.hpp"

int main() {
    DynamicLoader loader;
    AbsDerivative* der = nullptr;
    AbsSort* sorter = nullptr;

    loader.Load("./libreal1.so");
    der = loader.CreateDerivative();
    sorter = loader.CreateSort();

    while (true) {
        std::cout << "Введите, что хотите сделать" << std::endl;
        std::cout << "-1 Конец программы" << std::endl;
        std::cout << "0 Смена библиотеки" << std::endl;
        std::cout << "1 Производная" << std::endl;
        std::cout << "2 Сортировка" << std::endl;
        int cmd;
        std::cin >> cmd;

        if (cmd == 0) {
            std::cout << "Введите название библиотеки" << std::endl;
            std::string lib;
            std::cin >> lib;

            loader.DeleteDerivative(der);
            loader.DeleteSort(sorter);

            if (!loader.Load(lib)) {
                std::cout << "Load error: " << loader.Error() << std::endl;
                continue;
            }

            der = loader.CreateDerivative();
            sorter = loader.CreateSort();
        }
        else if (cmd == 1) {
            float a;
            float dx;
            std::cout << "Введите точку" << std::endl;
            std::cin >> a;
            std::cout << "Введите отклонение" << std::endl;
            std::cin >> dx;
            std::cout << der->Derivative(a, dx) << std::endl;
        }
        else if (cmd == 2) {
            int n;
            std::cout << "Введите размер массива" << std::endl;
            std::cin >> n;
            int * array = new int[n];

            size_t i = 0;
            std::cout << "Введите элементы массива" << std::endl;
            for (i = 0; i < n; ++i) std::cin >> array[i];

            sorter->Sort(array, n);

            for (int i = 0; i < n; i++) std::cout << array[i] << " ";
            std::cout << std::endl;

            delete[] array;
        }
        else if (cmd == -1) break;
    }

    loader.DeleteDerivative(der);
    loader.DeleteSort(sorter);
}
