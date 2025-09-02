#include "keyboard.hpp"

char Keyboard::keyboardState[MAX_KEYBOARD_KEYS] = {0};
char Keyboard::keyboardStateLastFrame[MAX_KEYBOARD_KEYS] = {0};

void Keyboard::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key < 0) return;

    if (action == GLFW_RELEASE) keyboardState[key] = 0;
    else if (action == GLFW_PRESS) keyboardState[key] = 1;
    else if (action == GLFW_REPEAT) keyboardState[key] = 1;
}