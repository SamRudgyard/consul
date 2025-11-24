#pragma once

#include "glm/glm.hpp"

#define MAX_MOUSE_BUTTONS 8

/** Simple mouse state container with positions, scroll and buttons. */
class Mouse
{
public:
    glm::vec2 position;
    glm::vec2 positionLastFrame;
    glm::vec2 scroll;
    bool buttons[MAX_MOUSE_BUTTONS];
    bool buttonsLastFrame[MAX_MOUSE_BUTTONS];
};