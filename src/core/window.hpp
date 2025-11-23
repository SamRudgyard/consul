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

/**
 * Global window state and utilities for the GLFW window and OpenGL viewport.
 * All fields are static and reflect the current window/context state.
 */
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

    /**
     * Toggle between windowed and fullscreen (not yet fully implemented).
     */
    static void ToggleFullscreen();

    /**
     * Set OpenGL viewport to the given size.
     * On high-DPI platforms, scales the viewport to match framebuffer size.
     * 
     * @param width  New viewport width in pixels.
     * @param height New viewport height in pixels.
     */
    static void SetupViewport(unsigned int width, unsigned int height);

    /**
     * Get the DPI scale for the window.
     * 
     * @returns vec2 containing the DPI scale factors for X and Y axes.
     */
    static glm::vec2 GetWindowScaleDPI();

    /**
     * Callback when the window is resized.
     * 
     * @param window GLFW window pointer.
     * @param width  New window width in pixels.
     * @param height New window height in pixels.
     */
    static void WindowSizeCallback(GLFWwindow* window, int width, int height);

    /**
     * Whether the user and/or application has requested the window be closed.
     * 
     * @returns true if the window should close; false otherwise.
     */
    static bool ShouldClose();

    /**
     * Swap the front/back buffers for the current window.
     */
    static void SwapBuffers() { glfwSwapBuffers(handle); };

    /**
     * Enable or disable vertical sync (vsync).
     * 
     * @param enabled True to enable vsync; false to disable.
     */
    static void SetVSync(bool enabled);
};