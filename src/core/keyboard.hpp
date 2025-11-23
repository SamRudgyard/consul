#pragma once

#include "glm/glm.hpp"
#include "GLFW/glfw3.h"

#define MAX_KEYBOARD_KEYS 512

/** Keyboard input state updated via GLFW key callback. */
class Keyboard
{
public:
    static char keyboardState[MAX_KEYBOARD_KEYS];
    static char keyboardStateLastFrame[MAX_KEYBOARD_KEYS];

    /**
     * GLFW key callback to update current key state array.
     * 
     * @param window   GLFW window pointer.
     * @param key      Keyboard key code.
     * @param scancode Platform-specific scancode.
     * @param action   Key action (press, release, repeat).
     * @param mods     Modifier keys bitfield.
     */
    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
};