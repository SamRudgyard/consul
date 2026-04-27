#pragma once

#include "core/profiling/profiler.hpp"
#include "core/window.hpp"
#include "core/time.hpp"
#include "input/input_system.hpp"

class EngineContext
{
public:
    Window window;
    Time time;
    Profiler profiler;
    InputSystem inputSystem;

    static EngineContext* get()
    {
        static EngineContext instance;
        return &instance;
    }

private:
    EngineContext() {}
    ~EngineContext() {}
};
