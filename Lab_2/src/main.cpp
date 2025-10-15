// src/main.cpp
#include "QuickSort.hpp"
#include <iostream>
#include <vector>
#include <limits>

int main() {
    int max_threads, array_size;

    std::cout << "Enter the maximum number of threads: ";
    while (!(std::cin >> max_threads) || max_threads <= 0) {
        std::cout << "Please enter a positive number: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    std::cout << "Enter the size of the array: ";
    while (!(std::cin >> array_size) || array_size <= 0) {
        std::cout << "Please enter a positive number: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    std::vector<int> arr(array_size);
    std::cout << "Enter " << array_size << " integers, one per line:" << std::endl;
    for (int i = 0; i < array_size; ++i) {
        while (!(std::cin >> arr[i])) {
            std::cout << "Invalid input. Enter integer #" << i + 1 << ": ";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }

    ParallelQuickSort sorter(arr, max_threads);
    sorter.sort();

    std::cout << "Sorted array: ";
    for (int i = 0; i < array_size - 1; ++i) {
        std::cout << arr[i] << ", ";
    }
    std::cout << arr[array_size - 1] << std::endl;

    return 0;
}