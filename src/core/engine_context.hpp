#pragma once

#include "core/window.hpp"
#include "core/time.hpp"
#include "core/ui/user_interface.hpp"
#include "input/input_system.hpp"

class EngineContext
{
public:
    Window window;
    Time time;
    InputSystem inputSystem;
    UserInterface ui;

    static EngineContext* get()
    {
        static EngineContext instance;
        return &instance;
    }

private:
    EngineContext() {}
    ~EngineContext() {}
};
