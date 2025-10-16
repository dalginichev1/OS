// src/main.cpp
#include "QuickSort.hpp"
#include <iostream>
#include <vector>
#include <limits>
#include <cstdlib>
#include <chrono>
#include <random>

void print_usage(const char* program_name) {
    std::cout << "Usage: " << program_name << " -t <number>" << std::endl;
    std::cout << "  -t <number>  Maximum number of threads to use (positive integer)" << std::endl;
}

void generate_random_array(std::vector<int>& arr, int size) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(1, 1000000); 
    
    arr.resize(size);
    for (int i = 0; i < size; ++i) {
        arr[i] = dis(gen);
    }
}

int main(int argc, char* argv[]) {
    int max_threads = -1;
    
    if (argc == 1) {
        print_usage(argv[0]);
        return 1;
    }
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-t") {
            if (i + 1 < argc) {
                try {
                    max_threads = std::stoi(argv[i + 1]);
                    if (max_threads <= 0) {
                        std::cerr << "Error: max-threads must be a positive number" << std::endl;
                        return 1;
                    }
                    i++;
                } catch (const std::exception& e) {
                    std::cerr << "Error: Invalid number for max-threads" << std::endl;
                    return 1;
                }
            } else {
                std::cerr << "Error: -t requires a value" << std::endl;
                print_usage(argv[0]);
                return 1;
            }
        } else if (arg == "--help" || arg == "-h") {
            print_usage(argv[0]);
            return 0;
        } else {
            std::cerr << "Error: Unknown argument '" << arg << "'" << std::endl;
            print_usage(argv[0]);
            return 1;
        }
    }
    
    if (max_threads == -1) {
        std::cerr << "Error: -t argument is required" << std::endl;
        print_usage(argv[0]);
        return 1;
    }

    int array_size;

    std::cout << "Enter the size of the array: ";
    while (!(std::cin >> array_size) || array_size <= 0) {
        std::cout << "Please enter a positive number: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    std::vector<int> arr;

    std::cout << "Generating random array of size " << array_size << "..." << std::endl;
    generate_random_array(arr, array_size);

    std::cout << std::endl;

    auto start_time = std::chrono::high_resolution_clock::now();
    
    ParallelQuickSort sorter(arr, max_threads);
    sorter.sort();

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

    std::cout << "\n=== Performance Metrics ===" << std::endl;
    std::cout << "Maximum threads specified: " << max_threads << std::endl;
    std::cout << "Array size: " << array_size << std::endl;
    std::cout << "Execution time: " << duration.count() << " microseconds" << std::endl;
    std::cout << "Execution time: " << duration.count() / 1000.0 << " milliseconds" << std::endl;
    std::cout << "Execution time: " << duration.count() / 1000000.0 << " seconds" << std::endl;

    std::cout << std::endl;

    return 0;
}