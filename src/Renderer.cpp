#include "Renderer.h"
#include "Config.h"
#include "Terminal.h"

#include <algorithm>
#include <sstream>
#include <iomanip>
#include <iostream>

namespace Renderer
{
    using namespace Config::Color;

    namespace
    {
        FrameCallback    g_onFrame    = nullptr;
        CompleteCallback g_onComplete = nullptr;
    }

    void setGuiCallbacks(FrameCallback onFrame, CompleteCallback onComplete)
    {
        g_onFrame    = std::move(onFrame);
        g_onComplete = std::move(onComplete);
    }

    void clearGuiCallbacks()
    {
        g_onFrame    = nullptr;
        g_onComplete = nullptr;
    }

    std::string buildBars(const std::vector<int>& arr, int a, int b,
                           int sortedBoundary, bool sortedIsPrefix)
    {
        std::stringstream output;
        const int n = static_cast<int>(arr.size());
        if (n == 0)
            return "";

        int maxValue = *std::max_element(arr.begin(), arr.end());
        if (maxValue <= 0)
            maxValue = 1;

        const int labelWidth = static_cast<int>(std::to_string(maxValue).length());

        auto colorFor = [&](int idx) -> const std::string&
        {
            const bool isSorted = sortedBoundary >= 0 &&
                (sortedIsPrefix ? idx < sortedBoundary : idx >= sortedBoundary);
            if (isSorted) return GREEN;
            if (idx == a || idx == b) return YELLOW;
            return CYAN;
        };

        // Histogram, top to bottom.
        for (int level = maxValue; level >= 0; --level)
        {
            output << std::setw(labelWidth) << level << " | ";
            for (int j = 0; j < n; ++j)
            {
                if (arr[j] >= level)
                    output << colorFor(j) << "X " << RESET;
                else
                    output << "  ";
            }
            output << "\n";
        }

        // Baseline.
        output << std::string(labelWidth, ' ') << "   ";
        for (int i = 0; i < n; ++i) output << "--";
        output << "\n";

        // Value labels (each padded to 2 chars to line up with "X ").
        output << std::string(labelWidth, ' ') << "   ";
        for (int i = 0; i < n; ++i)
        {
            std::string numStr = std::to_string(arr[i]);
            if (numStr.length() == 1) numStr += " ";
            output << colorFor(i) << numStr << RESET;
        }
        output << "\n";

        return output.str();
    }

    void renderFrame(const std::string& title, const std::string& message,
                      const std::vector<int>& arr, int a, int b,
                      int sortedBoundary, bool sortedIsPrefix)
    {
        if (g_onFrame)
        {
            g_onFrame(title, message, arr, a, b, sortedBoundary, sortedIsPrefix);
            return;
        }

        std::cout << "\033[1;1H\033[J";
        std::cout << "========== " << title << " ==========\n\n";
        if (!message.empty())
            std::cout << message << "\n\n";
        std::cout << buildBars(arr, a, b, sortedBoundary, sortedIsPrefix);
        Terminal::sleepStep();
    }

    void renderComplete(const std::string& title, const std::vector<int>& arr,
                         long long comparisons, long long swapsOrShifts,
                         const std::string& secondMetricLabel)
    {
        if (g_onComplete)
        {
            g_onComplete(title, arr, comparisons, swapsOrShifts, secondMetricLabel);
            return;
        }

        Terminal::clearScreen();
        std::cout << "========== " << title << " ==========\n\n";
        std::cout << "Sorting complete!\n\n";
        std::cout << buildBars(arr, -1, -1, static_cast<int>(arr.size()), true);
        std::cout << "\nComparisons: " << comparisons
                   << "\n" << secondMetricLabel << ": " << swapsOrShifts << "\n\n";
        std::cout << "Press Enter to continue...";
        std::cin.ignore();
        std::cin.get();
    }
}
