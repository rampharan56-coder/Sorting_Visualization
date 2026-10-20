#include "InputUtils.h"

#include <iostream>
#include <limits>
#include <random>

namespace InputUtils
{
    int readInt(const std::string& prompt)
    {
        int value;
        while (true)
        {
            std::cout << prompt;
            std::cin >> value;
            if (std::cin.fail())
            {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Invalid input, please enter a number.\n";
                continue;
            }
            return value;
        }
    }

    std::vector<int> readArrayManually(int size)
    {
        std::vector<int> data(size);
        for (int i = 0; i < size; ++i)
            data[i] = readInt("Enter [" + std::to_string(i + 1) + "] : ");
        return data;
    }

    std::vector<int> generateRandomArray(int size, int minVal, int maxVal)
    {
        static std::mt19937 rng(std::random_device{}());
        std::uniform_int_distribution<int> dist(minVal, maxVal);

        std::vector<int> data(size);
        for (int& v : data) v = dist(rng);
        return data;
    }
}
