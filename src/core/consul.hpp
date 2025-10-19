#pragma once

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "core/console/console.hpp"
#include "window.hpp"
#include "mouse.hpp"
#include "time.hpp"
#include "keyboard.hpp"

class Consul
{
public:
    Consul(const char* title, unsigned int width, unsigned int height, bool isFullscreen = false);
    Consul(const char* title, bool isFullscreen = false);
    ~Consul();

    bool Run();
    void Terminate();
    void VSync(bool enabled);

private:
    Console* console = Console::Get();
};