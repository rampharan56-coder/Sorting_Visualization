#include "Sorting.h"
#include "Renderer.h"

#include <chrono>
#include <utility>

namespace Sorting
{
    using Clock = std::chrono::steady_clock;

    // Small helper so every function starts/stops timing the same way.
    static double msSince(const Clock::time_point& start)
    {
        return std::chrono::duration<double, std::milli>(Clock::now() - start).count();
    }

    // ---------------- Bubble Sort ----------------
    SortStats bubbleSort(std::vector<int> arr, bool visualize)
    {
        auto start = Clock::now();
        SortStats stats;
        stats.algorithmName = "Bubble Sort";
        const int n = static_cast<int>(arr.size());

        if (visualize) Renderer::renderFrame("BUBBLE SORT", "Starting sort...", arr);

        for (int i = 0; i < n - 1; ++i)
        {
            bool swapped = false;
            if (visualize)
                Renderer::renderFrame("BUBBLE SORT",
                    "Pass " + std::to_string(i + 1) + " / " + std::to_string(n - 1),
                    arr, -1, -1, n - i, false);

            for (int j = 0; j < n - i - 1; ++j)
            {
                stats.comparisons++;
                if (visualize)
                    Renderer::renderFrame("BUBBLE SORT",
                        "Comparing " + std::to_string(arr[j]) + " and " + std::to_string(arr[j + 1]),
                        arr, j, j + 1, n - i, false);

                if (arr[j] > arr[j + 1])
                {
                    std::swap(arr[j], arr[j + 1]);
                    stats.swapsOrShifts++;
                    swapped = true;
                    if (visualize) Renderer::renderFrame("BUBBLE SORT", "Swapped!", arr, j, j + 1, n - i, false);
                }
            }
            if (!swapped)
            {
                if (visualize) Renderer::renderFrame("BUBBLE SORT", "No swaps -> already sorted!", arr, -1, -1, 0, false);
                break;
            }
        }

        if (visualize) Renderer::renderComplete("BUBBLE SORT", arr, stats.comparisons, stats.swapsOrShifts, "Swaps");
        stats.result = arr;
        stats.elapsedMs = msSince(start);
        return stats;
    }

    // ---------------- Insertion Sort ----------------
    SortStats insertionSort(std::vector<int> arr, bool visualize)
    {
        auto start = Clock::now();
        SortStats stats;
        stats.algorithmName = "Insertion Sort";
        const int n = static_cast<int>(arr.size());

        if (visualize) Renderer::renderFrame("INSERTION SORT", "Starting sort...", arr, -1, -1, 1, true);

        for (int i = 1; i < n; ++i)
        {
            int temp = arr[i];
            int j = i - 1;
            if (visualize)
                Renderer::renderFrame("INSERTION SORT",
                    "Pass " + std::to_string(i) + " / " + std::to_string(n - 1) +
                        " -- Inserting " + std::to_string(temp),
                    arr, i, -1, i, true);

            while (j >= 0)
            {
                stats.comparisons++;
                if (visualize)
                    Renderer::renderFrame("INSERTION SORT",
                        "Comparing " + std::to_string(arr[j]) + " and " + std::to_string(temp),
                        arr, j, i, i, true);

                if (arr[j] > temp)
                {
                    arr[j + 1] = arr[j];
                    stats.swapsOrShifts++;
                    j--;
                    if (visualize) Renderer::renderFrame("INSERTION SORT", "Shifted element", arr, j + 1, -1, i, true);
                }
                else break;
            }
            arr[j + 1] = temp;
            if (visualize) Renderer::renderFrame("INSERTION SORT", "Inserted " + std::to_string(temp), arr, j + 1, -1, i + 1, true);
        }

        if (visualize) Renderer::renderComplete("INSERTION SORT", arr, stats.comparisons, stats.swapsOrShifts, "Shifts");
        stats.result = arr;
        stats.elapsedMs = msSince(start);
        return stats;
    }

    // ---------------- Selection Sort ----------------
    SortStats selectionSort(std::vector<int> arr, bool visualize)
    {
        auto start = Clock::now();
        SortStats stats;
        stats.algorithmName = "Selection Sort";
        const int n = static_cast<int>(arr.size());

        if (visualize) Renderer::renderFrame("SELECTION SORT", "Starting sort...", arr, -1, -1, 0, true);

        for (int i = 0; i < n - 1; ++i)
        {
            int minIdx = i;
            if (visualize)
                Renderer::renderFrame("SELECTION SORT",
                    "Pass " + std::to_string(i + 1) + " / " + std::to_string(n - 1) + " -- searching for minimum...",
                    arr, minIdx, -1, i, true);

            for (int j = i + 1; j < n; ++j)
            {
                stats.comparisons++;
                if (visualize)
                    Renderer::renderFrame("SELECTION SORT",
                        "Current min " + std::to_string(arr[minIdx]) + ", comparing with " + std::to_string(arr[j]),
                        arr, minIdx, j, i, true);

                if (arr[j] < arr[minIdx])
                {
                    minIdx = j;
                    if (visualize) Renderer::renderFrame("SELECTION SORT", "New minimum: " + std::to_string(arr[minIdx]), arr, minIdx, -1, i, true);
                }
            }
            if (minIdx != i)
            {
                std::swap(arr[i], arr[minIdx]);
                stats.swapsOrShifts++;
                if (visualize) Renderer::renderFrame("SELECTION SORT", "Swapped into place", arr, i, minIdx, i + 1, true);
            }
        }

        if (visualize) Renderer::renderComplete("SELECTION SORT", arr, stats.comparisons, stats.swapsOrShifts, "Swaps");
        stats.result = arr;
        stats.elapsedMs = msSince(start);
        return stats;
    }

    // ---------------- Merge Sort ----------------
    static void mergeHelper(std::vector<int>& arr, int left, int mid, int right,
                             SortStats& stats, bool visualize)
    {
        std::vector<int> temp;
        temp.reserve(right - left + 1);
        int i = left, j = mid + 1;

        while (i <= mid && j <= right)
        {
            stats.comparisons++;
            if (visualize)
                Renderer::renderFrame("MERGE SORT",
                    "Merging: comparing " + std::to_string(arr[i]) + " and " + std::to_string(arr[j]),
                    arr, i, j);
            if (arr[i] <= arr[j]) temp.push_back(arr[i++]);
            else temp.push_back(arr[j++]);
            stats.swapsOrShifts++; // counts as a write into the merged buffer
        }
        while (i <= mid) { temp.push_back(arr[i++]); stats.swapsOrShifts++; }
        while (j <= right) { temp.push_back(arr[j++]); stats.swapsOrShifts++; }

        for (int k = 0; k < static_cast<int>(temp.size()); ++k)
            arr[left + k] = temp[k];

        if (visualize) Renderer::renderFrame("MERGE SORT", "Merged segment [" + std::to_string(left) + ".." + std::to_string(right) + "]", arr, left, right);
    }

    static void mergeSortHelper(std::vector<int>& arr, int left, int right,
                                 SortStats& stats, bool visualize)
    {
        if (left >= right) return;
        int mid = left + (right - left) / 2;
        mergeSortHelper(arr, left, mid, stats, visualize);
        mergeSortHelper(arr, mid + 1, right, stats, visualize);
        mergeHelper(arr, left, mid, right, stats, visualize);
    }

    SortStats mergeSort(std::vector<int> arr, bool visualize)
    {
        auto start = Clock::now();
        SortStats stats;
        stats.algorithmName = "Merge Sort";

        if (visualize) Renderer::renderFrame("MERGE SORT", "Starting sort...", arr);
        mergeSortHelper(arr, 0, static_cast<int>(arr.size()) - 1, stats, visualize);
        if (visualize) Renderer::renderComplete("MERGE SORT", arr, stats.comparisons, stats.swapsOrShifts, "Writes");

        stats.result = arr;
        stats.elapsedMs = msSince(start);
        return stats;
    }

    // ---------------- Quick Sort ----------------
    static int partition(std::vector<int>& arr, int low, int high, SortStats& stats, bool visualize)
    {
        int pivot = arr[high];
        int i = low - 1;

        for (int j = low; j < high; ++j)
        {
            stats.comparisons++;
            if (visualize)
                Renderer::renderFrame("QUICK SORT",
                    "Comparing " + std::to_string(arr[j]) + " with pivot " + std::to_string(pivot),
                    arr, j, high);
            if (arr[j] < pivot)
            {
                ++i;
                std::swap(arr[i], arr[j]);
                stats.swapsOrShifts++;
                if (visualize) Renderer::renderFrame("QUICK SORT", "Swapped", arr, i, j);
            }
        }
        std::swap(arr[i + 1], arr[high]);
        stats.swapsOrShifts++;
        if (visualize) Renderer::renderFrame("QUICK SORT", "Pivot placed", arr, i + 1, high);
        return i + 1;
    }

    static void quickSortHelper(std::vector<int>& arr, int low, int high, SortStats& stats, bool visualize)
    {
        if (low < high)
        {
            int p = partition(arr, low, high, stats, visualize);
            quickSortHelper(arr, low, p - 1, stats, visualize);
            quickSortHelper(arr, p + 1, high, stats, visualize);
        }
    }

    SortStats quickSort(std::vector<int> arr, bool visualize)
    {
        auto start = Clock::now();
        SortStats stats;
        stats.algorithmName = "Quick Sort";

        if (visualize) Renderer::renderFrame("QUICK SORT", "Starting sort...", arr);
        quickSortHelper(arr, 0, static_cast<int>(arr.size()) - 1, stats, visualize);
        if (visualize) Renderer::renderComplete("QUICK SORT", arr, stats.comparisons, stats.swapsOrShifts, "Swaps");

        stats.result = arr;
        stats.elapsedMs = msSince(start);
        return stats;
    }

    // ---------------- Heap Sort ----------------
    static void heapify(std::vector<int>& arr, int n, int i, SortStats& stats, bool visualize)
    {
        int largest = i, left = 2 * i + 1, right = 2 * i + 2;

        if (left < n) { stats.comparisons++; if (arr[left] > arr[largest]) largest = left; }
        if (right < n) { stats.comparisons++; if (arr[right] > arr[largest]) largest = right; }

        if (largest != i)
        {
            std::swap(arr[i], arr[largest]);
            stats.swapsOrShifts++;
            if (visualize) Renderer::renderFrame("HEAP SORT", "Sifting down", arr, i, largest);
            heapify(arr, n, largest, stats, visualize);
        }
    }

    SortStats heapSort(std::vector<int> arr, bool visualize)
    {
        auto start = Clock::now();
        SortStats stats;
        stats.algorithmName = "Heap Sort";
        const int n = static_cast<int>(arr.size());

        if (visualize) Renderer::renderFrame("HEAP SORT", "Building max-heap...", arr);
        for (int i = n / 2 - 1; i >= 0; --i)
            heapify(arr, n, i, stats, visualize);

        for (int i = n - 1; i > 0; --i)
        {
            std::swap(arr[0], arr[i]);
            stats.swapsOrShifts++;
            if (visualize) Renderer::renderFrame("HEAP SORT", "Moved max to end", arr, 0, i, i, false);
            heapify(arr, i, 0, stats, visualize);
        }

        if (visualize) Renderer::renderComplete("HEAP SORT", arr, stats.comparisons, stats.swapsOrShifts, "Swaps");
        stats.result = arr;
        stats.elapsedMs = msSince(start);
        return stats;
    }

    // ---------------- Shell Sort ----------------
    SortStats shellSort(std::vector<int> arr, bool visualize)
    {
        auto start = Clock::now();
        SortStats stats;
        stats.algorithmName = "Shell Sort";
        const int n = static_cast<int>(arr.size());

        if (visualize) Renderer::renderFrame("SHELL SORT", "Starting sort...", arr);

        for (int gap = n / 2; gap > 0; gap /= 2)
        {
            for (int i = gap; i < n; ++i)
            {
                int temp = arr[i];
                int j = i;
                while (j >= gap)
                {
                    stats.comparisons++;
                    if (visualize)
                        Renderer::renderFrame("SHELL SORT",
                            "Gap " + std::to_string(gap) + ": comparing " + std::to_string(arr[j - gap]) + " and " + std::to_string(temp),
                            arr, j - gap, j);
                    if (arr[j - gap] > temp)
                    {
                        arr[j] = arr[j - gap];
                        stats.swapsOrShifts++;
                        j -= gap;
                    }
                    else break;
                }
                arr[j] = temp;
            }
        }

        if (visualize) Renderer::renderComplete("SHELL SORT", arr, stats.comparisons, stats.swapsOrShifts, "Shifts");
        stats.result = arr;
        stats.elapsedMs = msSince(start);
        return stats;
    }
}
