#pragma once

#include "glm/glm.hpp"

struct Window
{
    glm::vec2 position = glm::vec2(0.f, 0.f);             // Top-left position of the window
    glm::vec2 monitorSize = glm::vec2(0.f, 0.f);          // Size (pixels) of the monitor the window is on
    glm::vec2 windowSize = glm::vec2(1280.f, 720.f);      // Size (pixels) of the window
    glm::vec2 framebufferSize = glm::vec2(1280.f, 720.f); // Size (pixels) of the framebuffer
    glm::vec2 contentScale = glm::vec2(1.f, 1.f);         // Content scale (for high-DPI displays)

    const char* title = "Consul Application";

    bool isFullscreen = false;      // Fullscreen window
    bool isVisible = true;          // Window is visible
    bool isResizable = true;        // Window can be resized
    bool isDecorated = true;        // Window has decorations (title bar, borders, etc.)
    bool isMinimised = false;       // Window is minimised
    bool isMaximised = false;       // Window is maximised
    bool shouldClose = false;       // Window should be closed
    bool useVSync = true;           // VSync enabled
    bool isFocused = true;          // Focused on creation
    bool isFloating = false;        // Always on top
    bool isTransparent = false;     // Window is transparent
    bool useMSAA = true;            // Use multisample anti-aliasing
};