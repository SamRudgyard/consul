#include "platform_glfw.hpp"

#ifndef NOMINMAX
#define NOMINMAX // To prevent Windows.h (a dependency of glad/glad.h) defining min/max macros that conflict with std::min/std::max
#endif

#include "core/console/console.hpp"
#include "input/input_system.hpp"
#include "GLFW/glfw3.h"

#include <map>
#include <stdexcept>
#include <string>

void PlatformGLFW::initialiseWindow()
{
    Console& console = Console::get();

    glfwSetErrorCallback(onError);

#if defined(__APPLE__)
    glfwInitHint(GLFW_COCOA_CHDIR_RESOURCES, GLFW_FALSE); // Don't change working directory to *.app/Contents/Resources, we want it to stay where the executable is
#endif

    if (!glfwInit()) {
        console.error("[GLFW] Failed to initialize GLFW");
        return;
    }

    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_FALSE); // Don't minimise window when losing focus

    glfwWindowHint(GLFW_VISIBLE, windowConfig->isVisible                     ? GLFW_TRUE : GLFW_FALSE); // Make the window visible
    glfwWindowHint(GLFW_RESIZABLE, windowConfig->isResizable                 ? GLFW_TRUE : GLFW_FALSE); // Allow window to be resized
    glfwWindowHint(GLFW_DECORATED, windowConfig->isDecorated                 ? GLFW_TRUE : GLFW_FALSE); // Enable window decorations (title bar, borders, close button, etc.)
    glfwWindowHint(GLFW_FOCUSED, windowConfig->isFocused                     ? GLFW_TRUE : GLFW_FALSE); // Make the window focused (i.e. ready to receive input)
    glfwWindowHint(GLFW_FLOATING, windowConfig->isFloating                   ? GLFW_TRUE : GLFW_FALSE); // Make the window floating (i.e. always on top)
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, windowConfig->isTransparent ? GLFW_TRUE : GLFW_FALSE); // Enable transparent framebuffer

#if defined(__APPLE__)
     glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_TRUE); // Enable high-DPI retina framebuffer on macOS
#endif

    if (windowConfig->useMSAA) {
        glfwWindowHint(GLFW_SAMPLES, 4); // Enable 4x MSAA
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#if defined(__APPLE__)
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Required on macOS
#endif

    GLFWmonitor* primaryMonitor = nullptr;
    if (windowConfig->isFullscreen) {
        console.log("[GLFW] Creating fullscreen window...");

        primaryMonitor = glfwGetPrimaryMonitor();
        if (!primaryMonitor) {
            console.error("[GLFW] Failed to get primary monitor for fullscreen mode");
            return;
        }

        const GLFWvidmode* videoMode = glfwGetVideoMode(primaryMonitor);
        if (!videoMode) {
            console.error("[GLFW] Failed to get video mode for primary monitor");
            return;
        }

        windowConfig->windowSize = glm::vec2((float)(videoMode->width), (float)(videoMode->height));

        windowConfig->position = glm::vec2(0.f, 0.f);
    }

    const bool maximise = (windowConfig->windowSize.x == 0 || windowConfig->windowSize.y == 0);
    // Creating the window requires non-zero width and height
    windowConfig->windowSize.x = std::max(windowConfig->windowSize.x, 1.f);
    windowConfig->windowSize.y = std::max(windowConfig->windowSize.y, 1.f);
    handle = glfwCreateWindow((int)windowConfig->windowSize.x, (int)windowConfig->windowSize.y, windowConfig->title, primaryMonitor, nullptr);
    if (!handle) {
        glfwTerminate();
        console.error("[GLFW] Failed to create GLFW window");
        return;
    }
    glfwSetWindowUserPointer(handle, this); // Allows access to this PlatformGLFW instance in static callbacks

    if (maximise) {
        console.log("[GLFW] Creating maximised window...");
        glfwMaximizeWindow(handle);
        int framebufferWidth, framebufferHeight;
        glfwGetFramebufferSize(handle, &framebufferWidth, &framebufferHeight);
        windowConfig->windowSize = glm::vec2((float)(framebufferWidth), (float)(framebufferHeight));
    }

    glfwMakeContextCurrent(handle);
    int error = glfwGetError(NULL);
    if (error != GLFW_NO_ERROR) {
        console.error("[GLFW] Failed to initialise GLFW: " + std::string(getGLFWErrorCodeAsString(error)));
    }

    if (windowConfig->isMinimised) {
        console.log("[GLFW] Minimising window...");
        glfwIconifyWindow(handle);
    }

    // Try to centre window
    GLFWmonitor* currentMonitor = getCurrentMonitor();
    int monitorX, monitorY, monitorWidth, monitorHeight;
    glfwGetMonitorWorkarea(currentMonitor, &monitorX, &monitorY, &monitorWidth, &monitorHeight);
    int posX = std::min(monitorX + (monitorWidth - (int)windowConfig->windowSize.x) / 2, monitorX);
    int posY = std::min(monitorY + (monitorHeight - (int)windowConfig->windowSize.y) / 2, monitorY);

    windowConfig->position = glm::vec2((float)posX, (float)posY);

    glfwSetWindowPos(handle, posX, posY);

    // Handle window config
    toggleVSync(windowConfig->useVSync);
    toggleFullscreen(windowConfig->isFullscreen);
    toggleResizable(windowConfig->isResizable);
    toggleDecorated(windowConfig->isDecorated);
    toggleMinimised(windowConfig->isMinimised);
    toggleVisible(windowConfig->isVisible);
    toggleFocused(windowConfig->isFocused);
    toggleFloating(windowConfig->isFloating);
    toggleTransparent(windowConfig->isTransparent);

    // Initialise window event callbacks
    glfwSetWindowSizeCallback(handle, onWindowResized);
    glfwSetWindowPosCallback(handle, onWindowPosChanged);
    glfwSetWindowMaximizeCallback(handle, onWindowMaximised);
    glfwSetWindowIconifyCallback(handle, onWindowMinimised);
    glfwSetWindowFocusCallback(handle, onWindowFocused);
    glfwSetKeyCallback(handle, onKeyInput);
    glfwSetCharCallback(handle, onCharInput);
    glfwSetMouseButtonCallback(handle, onMouseButtonInput);
    glfwSetCursorPosCallback(handle, onMouseMoved);
    glfwSetScrollCallback(handle, onMouseScrolled);
    glfwSetCursorEnterCallback(handle, onMouseEnterOrExitWindow);

    glfwSetInputMode(handle, GLFW_LOCK_KEY_MODS, GLFW_TRUE); // Ensure modifier key flags (i.e. caps lock) are passed to key callbacks
}

void PlatformGLFW::loadGraphics(IGraphics* graphics)
{
    graphics->loadGraphics((void*)glfwGetProcAddress);
}

void PlatformGLFW::pollEvents()
{
    glfwPollEvents();
}

void PlatformGLFW::swapBuffers()
{
    glfwSwapBuffers(handle);
}

bool PlatformGLFW::shouldClose()
{
    return glfwWindowShouldClose(handle);
}

double PlatformGLFW::getTime()
{
    return glfwGetTime();
}

void PlatformGLFW::terminate()
{
    glfwDestroyWindow(handle);
    glfwTerminate();
}

void PlatformGLFW::onError(int error, const char* description)
{
    std::string errorCode = PlatformGLFW::getGLFWErrorCodeAsString(error);
    std::string errorMsg = "[GLFW] Error " + errorCode + ": " + std::string(description);
    throw std::runtime_error(errorMsg);
}

void PlatformGLFW::onWindowResized(GLFWwindow* window, int width, int height)
{
    if (width <= 0 || height <= 0) {
        return; // When window is minimised GLFW may send a resize event with 0 width and/or height
    }

    PlatformGLFW* platform = static_cast<PlatformGLFW*>(glfwGetWindowUserPointer(window));
    platform->windowConfig->windowSize = glm::vec2((float)width, (float)height);

    Console::get().logOnDebug("[GLFW] Window resized to " + std::to_string(width) + "x" + std::to_string(height));
}

void PlatformGLFW::onWindowPosChanged(GLFWwindow* window, int xpos, int ypos)
{
    PlatformGLFW* platform = static_cast<PlatformGLFW*>(glfwGetWindowUserPointer(window));
    platform->windowConfig->position = glm::vec2((float)xpos, (float)ypos);

    Console::get().logOnDebug("[GLFW] Window position changed to ("
                    + std::to_string((int)platform->windowConfig->position.x) + ", "
                    + std::to_string((int)platform->windowConfig->position.y) +
                    ")");
}

void PlatformGLFW::onWindowMaximised(GLFWwindow* window, int maximised)
{
    PlatformGLFW* platform = static_cast<PlatformGLFW*>(glfwGetWindowUserPointer(window));
    platform->windowConfig->isMaximised = maximised;

    if (platform->windowConfig->isMaximised) {
        Console::get().logOnDebug("[GLFW] Window maximised");
    } else {
        Console::get().logOnDebug("[GLFW] Window restored from maximised state");
    }
}

void PlatformGLFW::onWindowMinimised(GLFWwindow* window, int iconified)
{
    PlatformGLFW* platform = static_cast<PlatformGLFW*>(glfwGetWindowUserPointer(window));
    platform->windowConfig->isMinimised = iconified;

    if (platform->windowConfig->isMinimised) {
        Console::get().logOnDebug("[GLFW] Window minimised");
    } else {
        Console::get().logOnDebug("[GLFW] Window restored from minimised state");
    }
}

void PlatformGLFW::onWindowFocused(GLFWwindow* window, int focused)
{
    PlatformGLFW* platform = static_cast<PlatformGLFW*>(glfwGetWindowUserPointer(window));
    platform->windowConfig->isFocused = focused;

    if (platform->windowConfig->isFocused) {
        Console::get().logOnDebug("[GLFW] Window focused");
    } else {
        Console::get().logOnDebug("[GLFW] Window lost focus");
    }
}

void PlatformGLFW::onKeyInput(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key < 0) { 
        return; // Requried as GLFW may send negative key codes for some inputs
    }

    InputSystem& input = InputSystem::get();

    if (action == GLFW_RELEASE) {
        input.setKeyUp(key);
    } else if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        input.setKeyDown(key);
    }

    if ((key == GLFW_KEY_CAPS_LOCK && mods & GLFW_MOD_CAPS_LOCK) ||
        (key == GLFW_KEY_NUM_LOCK && mods & GLFW_MOD_NUM_LOCK)) {
        input.setKeyDown(key);
    }

    if (key == (int)(input.getExitKey()) && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

void PlatformGLFW::onCharInput(GLFWwindow* window, unsigned int codepoint)
{
    // TODO: Handle character input (for text input fields, etc.)
}

void PlatformGLFW::onMouseButtonInput(GLFWwindow* window, int button, int action, int mods)
{
    InputSystem& input = InputSystem::get();

    if (action == GLFW_PRESS) {
        input.setMouseButtonDown(button);
    } else if (action == GLFW_RELEASE) {
        input.setMouseButtonUp(button);
    }
}

void PlatformGLFW::onMouseMoved(GLFWwindow* window, double xpos, double ypos)
{
    InputSystem& input = InputSystem::get();
    input.setMousePosition(glm::vec2((float)xpos, (float)ypos));
}

void PlatformGLFW::onMouseScrolled(GLFWwindow* window, double xoffset, double yoffset)
{
    InputSystem& input = InputSystem::get();
    input.setMouseScrollOffset(glm::vec2((float)xoffset, (float)yoffset));
}

void PlatformGLFW::onMouseEnterOrExitWindow(GLFWwindow* window, int entered)
{
    InputSystem& input = InputSystem::get();
    input.setMouseInsideWindow(entered == GLFW_TRUE);
}

const char* PlatformGLFW::getGLFWErrorCodeAsString(int errorCode)
{
    switch (errorCode) {
        case GLFW_NOT_INITIALIZED:        return "GLFW_NOT_INITIALIZED";
        case GLFW_NO_CURRENT_CONTEXT:     return "GLFW_NO_CURRENT_CONTEXT";
        case GLFW_INVALID_ENUM:           return "GLFW_INVALID_ENUM";
        case GLFW_INVALID_VALUE:          return "GLFW_INVALID_VALUE";
        case GLFW_OUT_OF_MEMORY:          return "GLFW_OUT_OF_MEMORY";
        case GLFW_API_UNAVAILABLE:        return "GLFW_API_UNAVAILABLE";
        case GLFW_VERSION_UNAVAILABLE:    return "GLFW_VERSION_UNAVAILABLE";
        case GLFW_PLATFORM_ERROR:         return "GLFW_PLATFORM_ERROR";
        case GLFW_FORMAT_UNAVAILABLE:     return "GLFW_FORMAT_UNAVAILABLE";
        case GLFW_NO_WINDOW_CONTEXT:      return "GLFW_NO_WINDOW_CONTEXT";
        default:                          return "UNKNOWN_GLFW_ERROR";
    }
}

const unsigned int PlatformGLFW::getCurrentMonitorNumber() const
{
    int monitorCount;
    glfwGetMonitors(&monitorCount);

    if (monitorCount <= 0) {
        return 0;
    }

    GLFWmonitor* currentMonitor = nullptr;

    GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);
    if (windowConfig->isFullscreen) {
        currentMonitor = glfwGetWindowMonitor(handle);
        for (int im = 0; im < monitorCount; ++im) {
            if (monitors[im] == currentMonitor) {
                return im;
            }
        }
    }

    int windowCentreX, windowCentreY;
    glfwGetWindowPos(handle, &windowCentreX, &windowCentreY);
    windowCentreX += (int)(windowConfig->windowSize.x / 2);
    windowCentreY += (int)(windowConfig->windowSize.y / 2);

    for (int im = 0; im < monitorCount; ++im) {
        int monitorX, monitorY;
        glfwGetMonitorPos(monitors[im], &monitorX, &monitorY);
        
        const GLFWvidmode* mode = glfwGetVideoMode(monitors[im]);
        if (!mode) {
            Console::get().error("[GLFW] Failed to get video mode for monitor " + std::to_string(im));
        }

        if (windowCentreX >= monitorX && windowCentreX <= monitorX + mode->width &&
            windowCentreY >= monitorY && windowCentreY <= monitorY + mode->height) {
            return im;
        }
    }

    return 0; // Default to primary monitor
}

GLFWmonitor* PlatformGLFW::getCurrentMonitor() const
{
    int im = getCurrentMonitorNumber();
    int monitorCount;
    GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);
    if (im >= monitorCount) {
        Console::get().error("[GLFW] Attempted to get monitor " + std::to_string(im) + " but only " + std::to_string(monitorCount) + " monitors detected");
    }
    return monitors[im];
}

void PlatformGLFW::toggleVSync(const bool enable)
{
    glfwSwapInterval(0);
    if (enable) {
        Console::get().log("[GLFW] Enabling VSync...");
        glfwSwapInterval(1);
    }

    windowConfig->useVSync = enable;
}

void PlatformGLFW::toggleFullscreen(const bool enable)
{
    if (enable) {
        GLFWmonitor* currentMonitor = getCurrentMonitor();
        glfwSetWindowMonitor(handle, currentMonitor, 0, 0, (int)windowConfig->windowSize.x, (int)windowConfig->windowSize.y, GLFW_DONT_CARE);
    } else {
        glfwSetWindowMonitor(handle, nullptr, (int)windowConfig->position.x, (int)windowConfig->position.y, (int)windowConfig->windowSize.x, (int)windowConfig->windowSize.y, GLFW_DONT_CARE);
    }

    windowConfig->isFullscreen = enable;
}

void PlatformGLFW::toggleResizable(const bool enable)
{
    glfwSetWindowAttrib(handle, GLFW_RESIZABLE, enable ? GLFW_TRUE : GLFW_FALSE);
    windowConfig->isResizable = enable;
}

void PlatformGLFW::toggleDecorated(const bool enable)
{
    glfwSetWindowAttrib(handle, GLFW_DECORATED, enable ? GLFW_TRUE : GLFW_FALSE);
    windowConfig->isDecorated = enable;
}

void PlatformGLFW::toggleMinimised(const bool enable)
{
    if (enable) {
        glfwIconifyWindow(handle);
    } else {
        glfwRestoreWindow(handle);
    }

    windowConfig->isMinimised = enable;
}

void PlatformGLFW::toggleMaximised(const bool enable)
{
    if (!windowConfig->isResizable) {
        return; // Can't maximise a non-resizable window
    }

    glfwSetWindowAttrib(handle, GLFW_MAXIMIZED, enable ? GLFW_TRUE : GLFW_FALSE);
    windowConfig->isMaximised = enable;
}

void PlatformGLFW::toggleVisible(const bool enable)
{
    if (enable) {
        glfwShowWindow(handle);
    } else {
        glfwHideWindow(handle);
    }

    windowConfig->isVisible = enable;
}

void PlatformGLFW::toggleFocused(const bool enable)
{
    glfwSetWindowAttrib(handle, GLFW_FOCUS_ON_SHOW, enable ? GLFW_TRUE : GLFW_FALSE);

    windowConfig->isFocused = enable;
}

void PlatformGLFW::toggleFloating(const bool enable)
{
    glfwSetWindowAttrib(handle, GLFW_FLOATING, enable ? GLFW_TRUE : GLFW_FALSE);

    windowConfig->isFloating = enable;
}

void PlatformGLFW::toggleTransparent(const bool enable)
{
    std::string isTransparent = windowConfig->isTransparent ? "enabled" : "disabled";
    Console::get().warn("[GLFW] Window transparency can only be set before window initialisation (currently " + isTransparent + ").");
}

void PlatformGLFW::toggleMSAA(const bool enable)
{
    std::string isMSAA = windowConfig->useMSAA ? "enabled" : "disabled";
    Console::get().warn("[GLFW] MSAA can only be set before window initialisation (currently " + isMSAA + ").");
}