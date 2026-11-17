#include "MainWindow.h"
#include "Config.h"
#include "Terminal.h"
#include "Renderer.h"
#include "Sorting.h"
#include "InputUtils.h"
#include "SortStats.h"

#include <QApplication>
#include <iostream>
#include <iomanip>
#include <vector>
#include <functional>
#include <string>
#include <cstring>

namespace
{
    struct MenuAlgorithm
    {
        std::string name;
        std::function<SortStats(std::vector<int>, bool)> run;
    };

    const std::vector<MenuAlgorithm>& algorithms()
    {
        static const std::vector<MenuAlgorithm> list = {
            {"Bubble Sort",    Sorting::bubbleSort},
            {"Insertion Sort", Sorting::insertionSort},
            {"Selection Sort", Sorting::selectionSort},
            {"Merge Sort",     Sorting::mergeSort},
            {"Quick Sort",     Sorting::quickSort},
            {"Heap Sort",      Sorting::heapSort},
            {"Shell Sort",     Sorting::shellSort},
        };
        return list;
    }

    std::vector<int> promptForArray()
    {
        int size = InputUtils::readInt("Enter array size (max " +
            std::to_string(Config::kRecommendedMaxSize) + " for best display): ");
        if (size <= 0)
        {
            std::cout << "Invalid input! The number must be greater than zero.\n";
            return {};
        }
        if (size > Config::kRecommendedMaxSize)
        {
            std::cout << "Warning: Large arrays may not display well in the terminal.\n";
            std::cout << "Consider using a smaller size (10-20) for best visualization.\n";
        }

        std::cout << "1. Enter values manually\n2. Generate random values\nChoose: ";
        int mode;
        std::cin >> mode;

        if (mode == 2)
            return InputUtils::generateRandomArray(size, Config::kDefaultRandomMin, Config::kDefaultRandomMax);
        return InputUtils::readArrayManually(size);
    }

    void printAlgorithmMenu()
    {
        const auto& algos = algorithms();
        for (size_t i = 0; i < algos.size(); ++i)
            std::cout << (i + 1) << ". " << algos[i].name << "\n";
        std::cout << (algos.size() + 1) << ". Benchmark ALL algorithms (no animation)\n";
        std::cout << (algos.size() + 2) << ". Change animation speed (currently " << Config::delayMs << " ms/step)\n";
        std::cout << "0. Exit\n\n";
    }

    void benchmarkAll(const std::vector<int>& data)
    {
        Terminal::clearScreen();
        std::cout << "========== BENCHMARK: ALL ALGORITHMS ==========\n\n";
        std::cout << std::left
                   << std::setw(18) << "Algorithm"
                   << std::setw(14) << "Comparisons"
                   << std::setw(14) << "Swaps/Shifts"
                   << std::setw(12) << "Time (ms)" << "\n";
        std::cout << std::string(58, '-') << "\n";

        for (const auto& algo : algorithms())
        {
            SortStats stats = algo.run(data, /*visualize=*/false);
            std::cout << std::left
                       << std::setw(18) << stats.algorithmName
                       << std::setw(14) << stats.comparisons
                       << std::setw(14) << stats.swapsOrShifts
                       << std::setw(12) << std::fixed << std::setprecision(3) << stats.elapsedMs
                       << "\n";
        }

        std::cout << "\nPress Enter to continue...";
        std::cin.ignore();
        std::cin.get();
    }

    int runCliMode()
    {
        Renderer::clearGuiCallbacks();
        Terminal::enableAnsi();

        std::vector<int> data = promptForArray();
        if (data.empty())
            return 1;

        const auto& algos = algorithms();
        const int benchmarkOption = static_cast<int>(algos.size()) + 1;
        const int speedOption     = static_cast<int>(algos.size()) + 2;

        int option = -1;
        do
        {
            Terminal::clearScreen();
            std::cout << "=================================\n";
            std::cout << "       SORTING VISUALIZER        \n";
            std::cout << "=================================\n\n";
            std::cout << "Current Array (size: " << data.size() << "):\n";
            std::cout << Renderer::buildBars(data) << "\n";

            printAlgorithmMenu();
            option = InputUtils::readInt("Choose: ");

            if (option >= 1 && option <= static_cast<int>(algos.size()))
            {
                algos[option - 1].run(data, /*visualize=*/true);
            }
            else if (option == benchmarkOption)
            {
                benchmarkAll(data);
            }
            else if (option == speedOption)
            {
                Config::delayMs = InputUtils::readInt("Enter delay in milliseconds (e.g. 200): ");
                if (Config::delayMs < 0) Config::delayMs = 0;
            }
            else if (option == 0)
            {
                std::cout << "Goodbye!\n";
            }
            else
            {
                std::cout << "Invalid input! Please enter a valid option.\n";
                Terminal::sleepStep();
            }
        } while (option != 0);

        return 0;
    }
}

int main(int argc, char* argv[])
{
    for (int i = 1; i < argc; ++i)
    {
        if (std::strcmp(argv[i], "--cli") == 0)
            return runCliMode();
    }

    QApplication app(argc, argv);
    MainWindow window;
    window.show();
    return app.exec();
}
