#pragma once

#include "core/window.hpp"
#include "core/time.hpp"
#include "input/input_system.hpp"

class EngineContext
{
public:
    Window window;
    Time time;
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