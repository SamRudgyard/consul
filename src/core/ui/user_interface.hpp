#pragma once

#include <vector>

#include "performance_window.hpp"

/**
 * Lightweight ImGui wrapper that manages renderable UI windows.
 */
class UserInterface
{
public:
    UserInterface();
    
    void update();

    PerformanceWindow& getPerformanceWindow() { return performanceWindow; }

private:
    PerformanceWindow performanceWindow;
    std::vector<UIWindow*> windows;
};
