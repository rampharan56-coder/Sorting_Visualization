#pragma once
#include <string>

// Global, tweakable settings for the whole app live here so nothing
// is hard-coded / scattered across translation units.
namespace Config
{
    // Mutable at runtime (menu option "change animation speed").
    extern int delayMs;

    namespace Color
    {
        const std::string RESET  = "\033[0m";
        const std::string CYAN   = "\033[36m";
        const std::string YELLOW = "\033[33m";
        const std::string RED    = "\033[31m";
        const std::string GREEN  = "\033[32m";
        const std::string DIM    = "\033[2m";
    }

    // Limits used for input validation / UX hints.
    constexpr int kRecommendedMaxSize = 30;
    constexpr int kDefaultRandomMin   = 1;
    constexpr int kDefaultRandomMax   = 99;
}
