#pragma once

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "window.hpp"
#include "mouse.hpp"
#include "time.hpp"
#include "keyboard.hpp"

#include "console/console.hpp"

class Consul
{
public:
    Consul(const char* title, unsigned int width, unsigned int height, bool isFullscreen = false);
    ~Consul() = default;

    bool Run();
    void Close();
    void VSync(bool enabled);

private:
    Console console;
};