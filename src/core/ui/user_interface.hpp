#pragma once

#include <memory>
#include <vector>

#ifndef CONSUL_CONSOLE_STDOUT
#include "console_window.hpp"
#endif
#include "performance_window.hpp"

/**
 * Lightweight ImGui wrapper that manages renderable UI windows.
 */
class UserInterface
{
public:
    UserInterface();
    
    void update();

#ifndef CONSUL_CONSOLE_STDOUT
    ConsoleWindow* getConsoleWindow() { return consoleWindow.get(); }
#endif
    PerformanceWindow& getPerformanceWindow() { return performanceWindow; }

private:
#ifndef CONSUL_CONSOLE_STDOUT
    std::unique_ptr<ConsoleWindow> consoleWindow;
#endif
    PerformanceWindow performanceWindow;
    std::vector<UIWindow*> windows;
};
