#pragma once

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "window.hpp"
#include "mouse.hpp"
#include "time.hpp"
#include "keyboard.hpp"

class App
{
public:
    App(const char* title, unsigned int width, unsigned int height, bool isFullscreen = false);
    ~App() = default;

    void Run() {};

private:
    bool shouldClose();
};