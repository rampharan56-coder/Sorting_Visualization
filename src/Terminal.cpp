#include "Terminal.h"
#include "Config.h"

#include <thread>
#include <chrono>
#include <iostream>

#ifdef _WIN32
  #ifndef NOMINMAX
    #define NOMINMAX
  #endif
  #include <windows.h>
#else
  #include <sys/ioctl.h>
  #include <unistd.h>
#endif

namespace Terminal
{
    int getWidth()
    {
#ifdef _WIN32
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi))
            return 80; // sane fallback
        return csbi.srWindow.Right - csbi.srWindow.Left + 1;
#else
        struct winsize w{};
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) != 0)
            return 80; // sane fallback
        return w.ws_col;
#endif
    }

    void enableAnsi()
    {
#ifdef _WIN32
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD mode = 0;
        GetConsoleMode(hOut, &mode);
        SetConsoleMode(hOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#endif
    }

    void clearScreen()
    {
        std::cout << "\033[2J\033[1;1H";
    }

    void sleepStep()
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(Config::delayMs));
    }
}
