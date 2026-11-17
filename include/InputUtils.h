#pragma once
#include <vector>
#include <string>

namespace InputUtils
{
    // Reads a validated integer, re-prompting on bad input.
    int readInt(const std::string& prompt);

    // Prompts the user for `size` numbers, one by one.
    std::vector<int> readArrayManually(int size);

    // Generates `size` random ints in [minVal, maxVal].
    std::vector<int> generateRandomArray(int size, int minVal, int maxVal);
}
