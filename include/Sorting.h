#pragma once
#include <vector>
#include "SortStats.h"

// All algorithms take the array BY VALUE (so the caller's original/menu copy
// is untouched), and take a `visualize` flag so the exact same function can
// be used for the animated demo AND for the fast, silent benchmark mode.
namespace Sorting
{
    SortStats bubbleSort(std::vector<int> arr, bool visualize = true);
    SortStats insertionSort(std::vector<int> arr, bool visualize = true);
    SortStats selectionSort(std::vector<int> arr, bool visualize = true);
    SortStats mergeSort(std::vector<int> arr, bool visualize = true);
    SortStats quickSort(std::vector<int> arr, bool visualize = true);
    SortStats heapSort(std::vector<int> arr, bool visualize = true);
    SortStats shellSort(std::vector<int> arr, bool visualize = true);
}
