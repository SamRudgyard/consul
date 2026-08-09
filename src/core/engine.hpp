#pragma once

#include "core/profiling/profiler.hpp"
#include "core/window.hpp"
#include "core/time.hpp"
#include "input/input_system.hpp"

class Engine
{
public:
    Window window;
    Time time;
    Profiler profiler;
    InputSystem inputSystem;

    static Engine* get()
    {
        static Engine instance;
        return &instance;
    }

private:
    Engine() {}
    ~Engine() {}
};
