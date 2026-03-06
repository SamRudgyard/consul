#include "platform_glfw.hpp"

#include "core/console/console.hpp"
#include "core/engine_context.hpp"
#include "GLFW/glfw3.h"

#include "imgui_impl_glfw.h"

#include <algorithm>
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

    Window& window = EngineContext::get()->window;
    glfwWindowHint(GLFW_VISIBLE, window.isVisible                     ? GLFW_TRUE : GLFW_FALSE); // Make the window visible
    glfwWindowHint(GLFW_RESIZABLE, window.isResizable                 ? GLFW_TRUE : GLFW_FALSE); // Allow window to be resized
    glfwWindowHint(GLFW_DECORATED, window.isDecorated                 ? GLFW_TRUE : GLFW_FALSE); // Enable window decorations (title bar, borders, close button, etc.)
    glfwWindowHint(GLFW_FOCUSED, window.isFocused                     ? GLFW_TRUE : GLFW_FALSE); // Make the window focused (i.e. ready to receive input)
    glfwWindowHint(GLFW_FLOATING, window.isFloating                   ? GLFW_TRUE : GLFW_FALSE); // Make the window floating (i.e. always on top)
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, window.isTransparent ? GLFW_TRUE : GLFW_FALSE); // Enable transparent framebuffer

#if defined(__APPLE__)
     glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_TRUE); // Enable high-DPI retina framebuffer on macOS
#endif

    if (window.useMSAA) {
        glfwWindowHint(GLFW_SAMPLES, 4); // Enable 4x MSAA
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#if defined(__APPLE__)
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Required on macOS
#endif

    GLFWmonitor* primaryMonitor = nullptr;
    if (window.isFullscreen) {
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

        window.windowSize = glm::vec2((float)(videoMode->width), (float)(videoMode->height));

        window.position = glm::vec2(0.f, 0.f);
    }

    // Creating the window requires non-zero width and height
    window.windowSize.x = std::max(window.windowSize.x, 1.f);
    window.windowSize.y = std::max(window.windowSize.y, 1.f);
    handle = glfwCreateWindow((int)window.windowSize.x, (int)window.windowSize.y, window.title, primaryMonitor, nullptr);
    if (!handle) {
        glfwTerminate();
        console.error("[GLFW] Failed to create GLFW window");
        return;
    }
    glfwSetWindowUserPointer(handle, context); // Allows access to the EngineContext instance in static callbacks

    int windowWidth, framebufferWidth, windowHeight, framebufferHeight;
    glfwGetWindowSize(handle, &windowWidth, &windowHeight);
    window.windowSize = glm::vec2((float)(windowWidth), (float)(windowHeight));
    glfwGetFramebufferSize(handle, &framebufferWidth, &framebufferHeight);
    window.framebufferSize = glm::vec2((float)(framebufferWidth), (float)(framebufferHeight));
    glfwGetWindowContentScale(handle, &window.contentScale.x, &window.contentScale.y);

    glfwMakeContextCurrent(handle);
    int error = glfwGetError(NULL);
    if (error != GLFW_NO_ERROR) {
        console.error("[GLFW] Failed to initialise GLFW: " + std::string(getGLFWErrorCodeAsString(error)));
    }

    // Try to centre window
    if (!window.isFullscreen) {
        GLFWmonitor* currentMonitor = getCurrentMonitor();
        int monitorX, monitorY, monitorWidth, monitorHeight;
        glfwGetMonitorWorkarea(currentMonitor, &monitorX, &monitorY, &monitorWidth, &monitorHeight);
        int posX = std::max(monitorX + (monitorWidth - (int)window.windowSize.x) / 2, monitorX);
        int posY = std::max(monitorY + (monitorHeight - (int)window.windowSize.y) / 2, monitorY);

        window.position = glm::vec2((float)posX, (float)posY);
        glfwSetWindowPos(handle, posX, posY);
    }

    // Handle window config
    toggleVSync(window.useVSync);
    toggleFullscreen(window.isFullscreen);
    toggleResizable(window.isResizable);
    toggleDecorated(window.isDecorated);
    toggleMinimised(window.isMinimised);
    toggleMaximised(window.isMaximised);
    toggleVisible(window.isVisible);
    toggleFocused(window.isFocused);
    toggleFloating(window.isFloating);

    // Initialise window event callbacks
    glfwSetFramebufferSizeCallback(handle, onFramebufferResized);
    glfwSetWindowSizeCallback(handle, onWindowResized);
    glfwSetWindowPosCallback(handle, onWindowPosChanged);
    glfwSetWindowMaximizeCallback(handle, onWindowMaximised);
    glfwSetWindowIconifyCallback(handle, onWindowMinimised);
    glfwSetWindowFocusCallback(handle, onWindowFocused);
    glfwSetWindowContentScaleCallback(handle, onContentScaleChanged);
    glfwSetKeyCallback(handle, onKeyInput);
    glfwSetCharCallback(handle, onCharInput);
    glfwSetMouseButtonCallback(handle, onMouseButtonInput);
    glfwSetCursorPosCallback(handle, onMouseMoved);
    glfwSetScrollCallback(handle, onMouseScrolled);
    glfwSetCursorEnterCallback(handle, onMouseEnterOrExitWindow);

    glfwSetInputMode(handle, GLFW_LOCK_KEY_MODS, GLFW_TRUE); // Ensure modifier key flags (i.e. caps lock) are passed to key callbacks
}

void PlatformGLFW::initialiseImGui(GraphicsAPI gfxApi)
{
    switch (gfxApi) {
        case GraphicsAPI::OpenGL:
            ImGui_ImplGlfw_InitForOpenGL(handle, true);
            break;
        default:
            Console::get().error("[PlatformGLFW] Unknown graphics API for ImGui initialisation!");
            return;
    }
}

void PlatformGLFW::loadGraphics(IGraphics* graphics)
{
    graphics->loadGraphics((void*)glfwGetProcAddress);
}

void PlatformGLFW::pollEvents()
{
    const InputSystem& input = EngineContext::get()->inputSystem;
    glfwSetInputMode(handle, GLFW_CURSOR, input.getMouseVisibility() ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_HIDDEN);

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

void PlatformGLFW::onFramebufferResized(GLFWwindow* window, int width, int height)
{
    if (width <= 0 || height <= 0) {
        return; // When window is minimised GLFW may send a resize event with 0 width and/or height
    }

    EngineContext* context = static_cast<EngineContext*>(glfwGetWindowUserPointer(window));
    context->window.framebufferSize = glm::vec2((float)width, (float)height);

    Console::get().logOnDebug("[GLFW] Framebuffer resized to " + std::to_string(width) + "x" + std::to_string(height));
}

void PlatformGLFW::onWindowResized(GLFWwindow* window, int width, int height)
{
    if (width <= 0 || height <= 0) {
        return; // When window is minimised GLFW may send a resize event with 0 width and/or height
    }

    EngineContext* context = static_cast<EngineContext*>(glfwGetWindowUserPointer(window));
    context->window.windowSize = glm::vec2((float)width, (float)height);

    Console::get().logOnDebug("[GLFW] Window resized to " + std::to_string(width) + "x" + std::to_string(height));
}

void PlatformGLFW::onWindowPosChanged(GLFWwindow* window, int xpos, int ypos)
{
    EngineContext* context = static_cast<EngineContext*>(glfwGetWindowUserPointer(window));
    context->window.position = glm::vec2((float)xpos, (float)ypos);

    Console::get().logOnDebug("[GLFW] Window position changed to ("
                    + std::to_string((int)context->window.position.x) + ", "
                    + std::to_string((int)context->window.position.y) +
                    ")");
}

void PlatformGLFW::onWindowMaximised(GLFWwindow* window, int maximised)
{
    EngineContext* context = static_cast<EngineContext*>(glfwGetWindowUserPointer(window));
    context->window.isMaximised = maximised;

    if (context->window.isMaximised) {
        Console::get().logOnDebug("[GLFW] Window maximised");
    } else {
        Console::get().logOnDebug("[GLFW] Window restored from maximised state");
    }
}

void PlatformGLFW::onWindowMinimised(GLFWwindow* window, int iconified)
{
    EngineContext* context = static_cast<EngineContext*>(glfwGetWindowUserPointer(window));
    context->window.isMinimised = iconified;

    if (context->window.isMinimised) {
        Console::get().logOnDebug("[GLFW] Window minimised");
    } else {
        Console::get().logOnDebug("[GLFW] Window restored from minimised state");
    }
}

void PlatformGLFW::onWindowFocused(GLFWwindow* window, int focused)
{
    EngineContext* context = static_cast<EngineContext*>(glfwGetWindowUserPointer(window));
    context->window.isFocused = focused;

    if (context->window.isFocused) {
        Console::get().logOnDebug("[GLFW] Window focused");
    } else {
        Console::get().logOnDebug("[GLFW] Window lost focus");
    }
}

void PlatformGLFW::onContentScaleChanged(GLFWwindow* window, float xscale, float yscale)
{
    EngineContext* context = static_cast<EngineContext*>(glfwGetWindowUserPointer(window));
    context->window.contentScale = glm::vec2(xscale, yscale);

    Console::get().logOnDebug("[GLFW] Window content scale changed to ("
                    + std::to_string(xscale) + ", "
                    + std::to_string(yscale) +
                    ")");
}

void PlatformGLFW::onKeyInput(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key < 0) { 
        return; // Requried as GLFW may send negative key codes for some inputs
    }

    const KeyboardKey keyEnum = (KeyboardKey)key;

    InputSystem& input = EngineContext::get()->inputSystem;

    if (action == GLFW_RELEASE) {
        input.setKeyUp(keyEnum);
    } else if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        input.setKeyDown(keyEnum);
    }

    if ((key == GLFW_KEY_CAPS_LOCK && mods & GLFW_MOD_CAPS_LOCK) ||
        (key == GLFW_KEY_NUM_LOCK && mods & GLFW_MOD_NUM_LOCK)) {
        input.setKeyDown(keyEnum);
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
    InputSystem& input = EngineContext::get()->inputSystem;
    const MouseButton buttonEnum = (MouseButton)button;

    if (action == GLFW_PRESS) {
        input.setMouseButtonDown(buttonEnum);
    } else if (action == GLFW_RELEASE) {
        input.setMouseButtonUp(buttonEnum);
    }
}

void PlatformGLFW::onMouseMoved(GLFWwindow* window, double xpos, double ypos)
{
    InputSystem& input = EngineContext::get()->inputSystem;
    input.setMousePosition(glm::vec2((float)xpos, (float)ypos));
}

void PlatformGLFW::onMouseScrolled(GLFWwindow* window, double xoffset, double yoffset)
{
    InputSystem& input = EngineContext::get()->inputSystem;
    input.setMouseScrollOffset(glm::vec2((float)xoffset, (float)yoffset));
}

void PlatformGLFW::onMouseEnterOrExitWindow(GLFWwindow* window, int entered)
{
    InputSystem& input = EngineContext::get()->inputSystem;
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

void PlatformGLFW::setMousePosition(unsigned int x, unsigned int y)
{
    glfwSetCursorPos(handle, (double)x, (double)y);
}

const unsigned int PlatformGLFW::getCurrentMonitorNumber() const
{
    int monitorCount;
    glfwGetMonitors(&monitorCount);

    if (monitorCount <= 0) {
        return 0;
    }

    Window& window = EngineContext::get()->window;

    GLFWmonitor* currentMonitor = nullptr;

    GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);
    if (window.isFullscreen) {
        currentMonitor = glfwGetWindowMonitor(handle);
        for (int im = 0; im < monitorCount; ++im) {
            if (monitors[im] == currentMonitor) {
                return im;
            }
        }
    }

    int windowCentreX, windowCentreY;
    glfwGetWindowPos(handle, &windowCentreX, &windowCentreY);
    windowCentreX += (int)(window.windowSize.x / 2);
    windowCentreY += (int)(window.windowSize.y / 2);

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
    glfwSwapInterval(enable ? 1 : 0);
    std::string vsyncStatus = enable ? "enabled" : "disabled";
    Console::get().logOnDebug("[GLFW] VSync " + vsyncStatus);

    EngineContext::get()->window.useVSync = enable;
}

void PlatformGLFW::toggleFullscreen(const bool enable)
{
    Window& window = EngineContext::get()->window;

    if (enable) {
        GLFWmonitor* currentMonitor = getCurrentMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(currentMonitor);
        if (!mode) {
            Console::get().error("[GLFW] Failed to get video mode for current monitor");
            return;
        }
        glfwSetWindowMonitor(handle, currentMonitor, 0, 0, mode->width, mode->height, mode->refreshRate);
    } else {
        glfwSetWindowMonitor(handle, nullptr, (int)window.position.x, (int)window.position.y, (int)window.windowSize.x, (int)window.windowSize.y, GLFW_DONT_CARE);
    }

    window.isFullscreen = enable;
}

void PlatformGLFW::toggleResizable(const bool enable)
{
    glfwSetWindowAttrib(handle, GLFW_RESIZABLE, enable ? GLFW_TRUE : GLFW_FALSE);
    EngineContext::get()->window.isResizable = enable;
}

void PlatformGLFW::toggleDecorated(const bool enable)
{
    glfwSetWindowAttrib(handle, GLFW_DECORATED, enable ? GLFW_TRUE : GLFW_FALSE);
    EngineContext::get()->window.isDecorated = enable;
}

void PlatformGLFW::toggleMinimised(const bool enable)
{
    if (enable) {
        glfwIconifyWindow(handle);
    } else {
        glfwRestoreWindow(handle);
    }

    EngineContext::get()->window.isMinimised = enable;
}

void PlatformGLFW::toggleMaximised(const bool enable)
{
    if (!EngineContext::get()->window.isResizable) {
        return; // Can't maximise a non-resizable window
    }

    if (enable) {
        glfwMaximizeWindow(handle);
    } else {
        glfwRestoreWindow(handle);
    }

    EngineContext::get()->window.isMaximised = enable;

    // Update window and framebuffer
    int winWidth, fbWidth, winHeight, fbHeight;
    glfwGetWindowSize(handle, &winWidth, &winHeight);
    glfwGetFramebufferSize(handle, &fbWidth, &fbHeight);
    EngineContext::get()->window.windowSize = glm::vec2((float)winWidth, (float)winHeight);
    EngineContext::get()->window.framebufferSize = glm::vec2((float)fbWidth, (float)fbHeight);
}

void PlatformGLFW::toggleVisible(const bool enable)
{
    if (enable) {
        glfwShowWindow(handle);
    } else {
        glfwHideWindow(handle);
    }

    EngineContext::get()->window.isVisible = enable;
}

void PlatformGLFW::toggleFocused(const bool enable)
{
    glfwSetWindowAttrib(handle, GLFW_FOCUS_ON_SHOW, enable ? GLFW_TRUE : GLFW_FALSE);

    EngineContext::get()->window.isFocused = enable;
    
    if (enable) {
        glfwFocusWindow(handle);
    }
}

void PlatformGLFW::toggleFloating(const bool enable)
{
    glfwSetWindowAttrib(handle, GLFW_FLOATING, enable ? GLFW_TRUE : GLFW_FALSE);

    EngineContext::get()->window.isFloating = enable;
}

void PlatformGLFW::toggleTransparent(const bool enable)
{
    std::string isTransparent = EngineContext::get()->window.isTransparent ? "enabled" : "disabled";
    Console::get().warn("[GLFW] Window transparency can only be set before window initialisation (currently " + isTransparent + ").");
}

void PlatformGLFW::toggleMSAA(const bool enable)
{
    std::string isMSAA = EngineContext::get()->window.useMSAA ? "enabled" : "disabled";
    Console::get().warn("[GLFW] MSAA can only be set before window initialisation (currently " + isMSAA + ").");
}