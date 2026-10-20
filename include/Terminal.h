#pragma once

// Thin, platform-independent wrapper around the handful of terminal
// operations the visualizer needs (Windows vs POSIX differ under the hood).
namespace Terminal
{
    int  getWidth();
    void enableAnsi();   // no-op on POSIX, needed on Windows consoles
    void clearScreen();
    void sleepStep();    // sleeps for Config::delayMs
}
