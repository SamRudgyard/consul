#include "consul.hpp"

#include "utils.hpp"

Consul::Consul(const char* title, unsigned int width, unsigned int height, bool isFullscreen)
{
    Log("[Consul] Initialising Game Engine...");

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

    Window::handle = glfwCreateWindow(width, height, title, monitor, nullptr);

    if (!Window::handle) {
        glfwTerminate();
        Error("[Consul] Failed to create GLFW window");
    }

    glfwMakeContextCurrent(Window::handle);

    glfwSwapInterval(Window::vsyncEnabled ? 1 : 0);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) Error("[Consul] Failed to initialize GLAD");
    glfwSetKeyCallback(Window::handle, Keyboard::KeyCallback);
    glfwSetWindowSizeCallback(Window::handle, Window::WindowSizeCallback);
    // glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

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
   
    Window::SetupViewport(width, height);

    Log("[Consul] OpenGL initialised successfully");

    Time::frameCount = 0;
    Window::shouldClose = false;
}
void Consul::VSync(bool enabled)
{
    Window::SetVSync(enabled);
}

bool Consul::Run()
{
    Time::NewFrame();

    glfwPollEvents();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Window::SwapBuffers();

    return !Window::ShouldClose();
}

void Consul::Close()
{
    Log("[Consul] Shutting down Game Engine...");

    glfwDestroyWindow(Window::handle);
    glfwTerminate();

    Log("[Consul] Shutdown complete.");
}