#pragma once

#include "window.hpp"
#include "mouse.hpp"
#include "time.hpp"
#include "keyboard.hpp"

class App
{
public:
    App(const char* title, unsigned int width, unsigned int height, bool isFullscreen = false);
    ~App();

    void Run();

private:
    static Window window;
    static Mouse mouse;
    static Time time;

    bool shouldClose();
};