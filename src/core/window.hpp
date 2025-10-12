#pragma once

#include "glm/glm.hpp"
#include "glad/glad.h"
#include "GLFW/glfw3.h"

// #include "imgui.h"
// #include "imgui_impl_glfw.h"
// #include "imgui_impl_opengl3.h"

enum WindowFlags
{
    WINDOW_RESIZABLE = 1 << 0,
    WINDOW_FULLSCREEN = 1 << 1,
    WINDOW_HIDDEN = 1 << 2,
    WINDOW_HIGH_DPI = 1 << 3
};

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

    static bool vsyncEnabled;

    static int screen;
    static int prevScreen;

    static glm::vec2 position;
    static glm::vec2 prevPosition;
    static glm::vec2 displaySize;
    static glm::vec2 screenSize;
    static glm::vec2 screenSizeLastFrame;

    static GLFWwindow* handle;
    // static ImGuiIO* io;

    static void ToggleFullscreen();
    static void SetupViewport(unsigned int width, unsigned int height);
    static glm::vec2 GetWindowScaleDPI();
    static void WindowSizeCallback(GLFWwindow* window, int width, int height);
    static bool ShouldClose();
    static void SwapBuffers() { glfwSwapBuffers(handle); };
    static void SetVSync(bool enabled);
};