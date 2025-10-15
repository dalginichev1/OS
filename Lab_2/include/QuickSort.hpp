#pragma once

#include <vector>

class ParallelQuickSort
{
    private:
        std::vector<int>& vector;
        int low;
        int high;
        int max_threads;

        int partition(int low, int high);
        void quicksort(int low, int high);
        static void* quicksort_thread(void* arg);
    public:
        ParallelQuickSort(std::vector<int>& vector, int threads, int start = 0, int end = -1);
        void sort();
};