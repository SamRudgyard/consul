#pragma once

#include "platform.hpp"

struct GLFWmonitor;
struct GLFWwindow;

class PlatformGLFW : public Platform
{
public:
    PlatformGLFW() : Platform() {}

    void initialiseWindow() override;

    void initialiseImGui(GraphicsAPI gfxApi) override;

    void initialiseGraphics(const Renderer& renderer) override;

    void pollEvents() override;

    void swapBuffers() override;

    bool shouldClose() override;

    double getTime() override;

    void setMousePosition(unsigned int x, unsigned int y) override;

    void terminate() override;

    static void onError(int error, const char* description);
    static void onFramebufferResized(GLFWwindow* window, int width, int height);
    static void onWindowResized(GLFWwindow* window, int width, int height);
    static void onWindowPosChanged(GLFWwindow* window, int xpos, int ypos);
    static void onWindowMaximised(GLFWwindow* window, int maximised);
    static void onWindowMinimised(GLFWwindow* window, int iconified);
    static void onWindowFocused(GLFWwindow* window, int focused);
    static void onContentScaleChanged(GLFWwindow* window, float xscale, float yscale);
    static void onKeyInput(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void onCharInput(GLFWwindow* window, unsigned int codepoint);
    static void onMouseButtonInput(GLFWwindow* window, int button, int action, int mods);
    static void onMouseMoved(GLFWwindow* window, double xpos, double ypos);
    static void onMouseScrolled(GLFWwindow* window, double xoffset, double yoffset);
    static void onMouseEnterOrExitWindow(GLFWwindow* window, int entered);

private:
    GLFWwindow* handle = nullptr;
    static const char* getGLFWErrorCodeAsString(int errorCode);

    const unsigned int getCurrentMonitorNumber() const;
    GLFWmonitor* getCurrentMonitor() const;
    
    void toggleVSync(const bool enable);
    void toggleFullscreen(const bool enable);
    void toggleResizable(const bool enable);
    void toggleDecorated(const bool enable);
    void toggleMinimised(const bool enable);
    void toggleMaximised(const bool enable);
    void toggleVisible(const bool enable);
    void toggleFocused(const bool enable);
    void toggleFloating(const bool enable);
    void toggleTransparent(const bool enable);
    void toggleMSAA(const bool enable);
};