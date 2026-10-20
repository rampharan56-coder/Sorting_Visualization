# Sorting Visualizer

A terminal-based, animated sorting algorithm visualizer written in C++17.

## Features
- 7 algorithms: Bubble, Insertion, Selection, Merge, Quick, Heap, Shell sort
- Live ANSI-colored bar-chart animation (yellow = active comparison, green = sorted region)
- Manual or random array input
- "Benchmark all" mode: runs every algorithm on the same array with no animation and prints a comparisons / swaps / time table
- Adjustable animation speed at runtime

## Build

### With CMake (recommended)
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
./sorting_visualizer        # or sorting_visualizer.exe on Windows
```

### Directly with g++
```bash
g++ -std=c++17 -O2 -Iinclude src/*.cpp -o sorting_visualizer -lpthread
./sorting_visualizer
```

## Project layout
```
include/        Public headers (one responsibility per file)
  Config.h        Colors, delay, tunable constants
  Terminal.h       Cross-platform terminal control (width, clear, ANSI, sleep)
  Renderer.h       Turns an array + highlight state into text frames
  SortStats.h      Uniform result struct returned by every algorithm
  Sorting.h        Declarations for all 7 sorting algorithms
  InputUtils.h     Validated input + random array generation
src/            Matching .cpp implementations + main.cpp (menu/orchestration)
  Config.cpp
  InputUtils.cpp
  main.cpp
  Renderer.cpp
  Sorting.cpp
  Terminal.cpp
CMakeLists.txt  Cross-platform build config
```

## Extending it
- **Add a new algorithm**: implement it in `Sorting.cpp` returning a `SortStats`,
  declare it in `Sorting.h`, then add one line to the `algorithms()` list in
  `main.cpp` — it automatically appears in both the animated menu and the
  benchmark table.
- **Add a new renderer** (e.g. export frames to HTML/SVG, or a GUI): implement
  the `Renderer` namespace functions differently; `Sorting.cpp` never touches
  `std::cout` directly, so it doesn't need to change.

