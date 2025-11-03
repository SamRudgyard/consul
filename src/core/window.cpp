#include "window.hpp"

#include "console/console.hpp"
#include "time.hpp"
#include "utils.hpp"

GLFWwindow* Window::handle = nullptr;
// ImGuiIO* Window::io = nullptr;
bool Window::isFullscreen = false;
unsigned int Window::width = 800;
unsigned int Window::height = 600;
const char* Window::title = nullptr;
unsigned int Window::flags = 0;
glm::vec2 Window::position = {0, 0};
glm::vec2 Window::prevPosition = {0, 0};
int Window::screen = 0;
int Window::prevScreen = 0;
bool Window::shouldClose = false;
bool Window::vsyncEnabled = true;

void Window::ToggleFullscreen()
{
    if (!isFullscreen)
    {
        prevPosition = position;
        prevScreen = screen;

    }
}

void Window::SetupViewport(unsigned int width, unsigned int height)
{
    Window::width = width;
    Window::height = height;

    glm::vec2 scale = { 1.0f, 1.0f };
#ifdef __APPLE__
    glfwGetWindowContentScale(Window::handle, &scale.x, &scale.y);
#endif
    glViewport(0, 0, width*scale.x, height*scale.y);        // Adjust viewport for high DPI displays
    // glOrtho(0.f, width, height, 0.f, 0.f, 1.f);             // Set orthographic projection
    glCheckError();
}

glm::vec2 Window::GetWindowScaleDPI()
{
    glm::vec2 scale = { 1.0f, 1.0f };
    glfwGetWindowContentScale(Window::handle, &scale.x, &scale.y);

    return scale;
}

void Window::WindowSizeCallback(GLFWwindow* window, int width, int height)
{
    SetupViewport(width, height);

    Window::width = width;
    Window::height = height;

    if (Window::isFullscreen) return;

    if (Window::flags & WINDOW_HIGH_DPI)
    {
        glm::vec2 scale = GetWindowScaleDPI();
        Window::width = (unsigned int)(width*scale.x);
        Window::height = (unsigned int)(height*scale.y);
    }
}

bool Window::ShouldClose()
{
    Window::shouldClose = glfwWindowShouldClose(Window::handle);

    if (Time::frameCount > 1000000)
    {
        // Log("[Consul] Frame count exceeded 1000000");
        Window::shouldClose = true;
    }

    return Window::shouldClose;
}

void Window::SetVSync(bool enabled)
{
    if (vsyncEnabled == enabled) return;
    
    vsyncEnabled = enabled;
    glfwSwapInterval(enabled ? 1 : 0);
}
