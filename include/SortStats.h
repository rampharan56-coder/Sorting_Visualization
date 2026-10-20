#pragma once
#include <string>
#include <vector>

// A single, uniform result type every algorithm returns. This is what makes
// the "benchmark all algorithms" menu option possible without special-casing
// each sort.
struct SortStats
{
    std::string algorithmName;
    long long   comparisons   = 0;
    long long   swapsOrShifts = 0; // swaps for bubble/selection, shifts for insertion, writes for merge, etc.
    double      elapsedMs     = 0.0;
    std::vector<int> result;       // the sorted array (useful in benchmark/no-visual mode)
};
