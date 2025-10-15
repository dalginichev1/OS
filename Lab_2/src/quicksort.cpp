#include "QuickSort.hpp"
#include <climits>
#include "Thread.hpp"
#include <cmath>

ParallelQuickSort::ParallelQuickSort(std::vector<int>& vector, int threads, int start, int end)
    : vector(vector), max_threads(threads > 0 ? threads : 1), low(start), high(end)
{
    if (max_threads > static_cast<int>(vector.size()))
    {
        max_threads = vector.size();
    }
}

int ParallelQuickSort::partition(int low, int high)
{
    int pivot = vector[high];
    int i = low - 1;
    int k = 0;
    int j = 0;
    for (j = low; j < high; ++j)
    {
        if (vector[j] < pivot)
        {
            ++i;
            k = vector[i];
            vector[i] = vector[j];
            vector[j] = k;
        }
    }

    k = vector[i + 1];
    vector[i + 1] = vector[high];
    vector[high] = k;

    return i + 1;
}

void ParallelQuickSort::quicksort(int low, int high)
{
    if (low < high)
    {
        int pivot = partition(low, high);
        quicksort(low, pivot - 1);
        quicksort(pivot + 1, high);
    }
}

void * ParallelQuickSort::quicksort_thread(void* arg)
{
    ParallelQuickSort* sorter = static_cast<ParallelQuickSort*>(arg);
    sorter->quicksort(sorter->low, sorter->high);
    delete sorter;
    return nullptr;
}

void ParallelQuickSort::sort()
{
    int n = vector.size();
    size_t i = 0;
    int pos = 0;

    std::vector<int> bounds(max_threads + 1);
    bounds[0] = 0;
    bounds[max_threads] = n;
    int part_size = n / max_threads;
    int other = n % max_threads;
    for (i = 1; i < max_threads; ++i)
    {
        bounds[i] = bounds[i - 1] + part_size + (i <= other ? 1 : 0);
    }

    std::vector<Thread> threads;
    threads.reserve(max_threads - 1);
    for (i = 0; i < max_threads; ++i)
    {
        if (bounds[i] >= bounds[i + 1]) continue;
        ParallelQuickSort* sorter = new ParallelQuickSort(vector, max_threads, bounds[i], bounds[i + 1] - 1);
        Thread th;

        if (th.create(quicksort_thread, sorter) != 0)
        {
            delete sorter;
            quicksort(bounds[i], bounds[i + 1] - 1);
        }
        else
        {
            threads.push_back(th);
        }
    }

    if (bounds[max_threads - 1] < bounds[max_threads])
    {
        quicksort(bounds[max_threads - 1], bounds[max_threads] - 1);
    }

    for (auto& th: threads)
    {
        th.join(nullptr);
    }

    std::vector<int> temp(n);
    std::vector<int> indices(max_threads);

    for (i = 0; i < max_threads; ++i)
    {
        indices[i] = bounds[i];
    }

    for (pos = 0; pos < n; ++pos)
    {
        int min_val = INT_MAX;
        int min_index = -1;

        for (i = 0; i < max_threads; ++i)
        {
            if (indices[i] < bounds[i + 1] && vector[indices[i]] < min_val)
            {
                min_val = vector[indices[i]];
                min_index = i;
            }
        }
        
        if (min_index != -1)
        {
            temp[pos] = min_val;
            indices[min_index]++;
        }
    }
    
    for (i = 0; i < n; ++i)
    {
        vector[i] = temp[i];
    }
}