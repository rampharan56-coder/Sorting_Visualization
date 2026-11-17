#pragma once
#include <vector>
#include <string>
#include <functional>

// Everything related to turning an array (+ highlight state) into text or GUI frames.
// Kept separate from the sorting logic so algorithms in Sorting.cpp don't know/care
// whether they're drawn in the terminal or in a Qt GUI window.
namespace Renderer
{
    using FrameCallback = std::function<void(
        const std::string& title,
        const std::string& message,
        const std::vector<int>& arr,
        int a, int b,
        int sortedBoundary, bool sortedIsPrefix)>;

    using CompleteCallback = std::function<void(
        const std::string& title,
        const std::vector<int>& arr,
        long long comparisons,
        long long swapsOrShifts,
        const std::string& secondMetricLabel)>;

    // Register / clear GUI callbacks. When set, renderFrame() and renderComplete()
    // forward frames to the Qt GUI instead of printing ANSI text to stdout.
    void setGuiCallbacks(FrameCallback onFrame, CompleteCallback onComplete);
    void clearGuiCallbacks();

    // Builds the ASCII/ANSI bar chart for one frame (used in CLI fallback mode).
    std::string buildBars(const std::vector<int>& arr,
                           int a = -1, int b = -1,
                           int sortedBoundary = -1, bool sortedIsPrefix = false);

    // Draws one full animation frame (title + message + bars) and sleeps
    // for Config::delayMs so the step is visible.
    void renderFrame(const std::string& title, const std::string& message,
                      const std::vector<int>& arr,
                      int a = -1, int b = -1,
                      int sortedBoundary = -1, bool sortedIsPrefix = false);

    // Displays the final "sorting complete" state with stats.
    void renderComplete(const std::string& title, const std::vector<int>& arr,
                         long long comparisons, long long swapsOrShifts,
                         const std::string& secondMetricLabel = "Swaps");
}
