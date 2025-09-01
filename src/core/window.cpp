#include "window.hpp"

#include "GLFW/glfw3.h"

void Window::ToggleFullscreen()
{
    if (!isFullscreen)
    {
        prevPosition = position;
        prevScreen = screen;

    }
}