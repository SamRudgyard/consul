#pragma once

#include <vector>

#include "console_window.hpp"
#include "performance_window.hpp"

/**
 * Lightweight ImGui wrapper that manages renderable UI windows.
 */
class UserInterface
{
public:
    UserInterface();
    
    void update();

    ConsoleWindow& getConsoleWindow() { return consoleWindow; }
    PerformanceWindow& getPerformanceWindow() { return performanceWindow; }

private:
    ConsoleWindow consoleWindow;
    PerformanceWindow performanceWindow;
    std::vector<UIWindow*> windows;
};
