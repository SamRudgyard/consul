#pragma once

#include "core/window_config.hpp"
#include "input/input_system.hpp"

class EngineContext
{
public:
    WindowConfig windowConfig;
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