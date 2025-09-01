#pragma once

#include "glm/glm.hpp"

#define MAX_MOUSE_BUTTONS 8

class Mouse
{
public:
    static glm::vec2 position;
    static glm::vec2 positionLastFrame;
    static glm::vec2 scroll;
    static bool buttons[MAX_MOUSE_BUTTONS];
    static bool buttonsLastFrame[MAX_MOUSE_BUTTONS];
};