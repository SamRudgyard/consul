#include "app.hpp"

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "utils.hpp"

App::App(const char* title, unsigned int width, unsigned int height, bool isFullscreen)
{
    Log("[ENGINE] Initialising Game Engine...");

    Window::title = title;
    Window::width = width;
    Window::height = height;
    Window::isFullscreen = isFullscreen;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_RESIZABLE, false);

    GLFWmonitor* monitor = nullptr;
    if (isFullscreen) {
        monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        width = mode->width;
        height = mode->height;
    }
    GLFWwindow* window = glfwCreateWindow(width, height, title, monitor, nullptr);
    glfwMakeContextCurrent(window);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) Error("[ENGINE] Failed to initialize GLAD");
    glfwSetKeyCallback(window, Keyboard::KeyCallback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Initialise OpenGL with our default settings
    // --------------------
    glDepthFunc(GL_LESS);                                   // Type of depth testing to apply
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);      // Colour blending, determines how pixel colours are combined
    glEnable(GL_BLEND);                                     // Enable colour blending (required for transparencies)
    glCullFace(GL_BACK);                                    // Cull back faces
    glFrontFace(GL_CCW);                                    // Front faces are counter clockwise
    glEnable(GL_CULL_FACE);                                 // Enable backface culling
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);                 // Enable seamless cubemap texture

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);                   // Set clear colour to black
    glClearDepth(1.0f);                                     // Set clear depth to farthest possible depth when glClear(GL_DEPTH_BUFFER_BIT) is called
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // Clear colour and depth buffers
    glm::vec2 scale = { 1.0f, 1.0f };
#ifdef __APPLE__
    glfwGetWindowContentScale(window, &scale.x, &scale.y);
#endif
    glViewport(0, 0, width*scale.x, height*scale.y);        // Adjust viewport for high DPI displays
    glOrtho(0.f, width, height, 0.f, 0.f, 1.f);             // Set orthographic projection

    Log("[ENGINE] OpenGL initialised successfully");

    Time::frameCount = 0;
    Window::shouldClose = false;
}

bool App::shouldClose()
{
    bool shouldClose = false;
    // TODO: logic to check if app should close
    return shouldClose;
}