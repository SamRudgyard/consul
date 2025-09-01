#pragma once

#include "glm/glm.hpp"
#include "GLFW/glfw3.h"

#define MAX_KEYBOARD_KEYS 512

class Keyboard
{
public:
    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
private:
    static char keyboardState[MAX_KEYBOARD_KEYS];
    static char keyboardStateLastFrame[MAX_KEYBOARD_KEYS];
};