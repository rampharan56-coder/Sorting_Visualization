#pragma once
#include <vector>
#include <string>

// Everything related to turning an array (+ highlight state) into text.
// Kept separate from the sorting logic so algorithms don't know/care how
// they're drawn, and so a future GUI/web renderer could swap this out.
namespace Renderer
{
    // Builds the ASCII/ANSI bar chart for one frame.
    //   a, b            -> indices to highlight (e.g. currently compared/swapped)
    //   sortedBoundary  -> index marking the sorted region
    //   sortedIsPrefix  -> true if [0, sortedBoundary) is sorted, false if [sortedBoundary, n) is
    std::string buildBars(const std::vector<int>& arr,
                           int a = -1, int b = -1,
                           int sortedBoundary = -1, bool sortedIsPrefix = false);

    // Draws one full animation frame (title + message + bars) and sleeps
    // for Config::delayMs so the step is visible.
    void renderFrame(const std::string& title, const std::string& message,
                      const std::vector<int>& arr,
                      int a = -1, int b = -1,
                      int sortedBoundary = -1, bool sortedIsPrefix = false);

    // Prints the final "sorting complete" screen with stats.
    void renderComplete(const std::string& title, const std::vector<int>& arr,
                         long long comparisons, long long swapsOrShifts,
                         const std::string& secondMetricLabel = "Swaps");
}
