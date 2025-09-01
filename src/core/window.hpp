#pragma once

#include "glm/glm.hpp"

class Window
{
public:
    static const char* title;
    static unsigned int width;
    static unsigned int height;
    static unsigned int flags;
    static bool isFullscreen;
    static bool shouldClose;
    static bool resizedLastFrame;

    static int screen;
    static int prevScreen;

    static glm::vec2 position;
    static glm::vec2 prevPosition;
    static glm::vec2 displaySize;
    static glm::vec2 screenSize;
    static glm::vec2 screenSizeLastFrame;

    static void ToggleFullscreen();
};