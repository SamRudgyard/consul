#include "consul.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "utils.hpp"

Consul::Consul(const char* title, unsigned int width, unsigned int height, bool isFullscreen)
{
    console.Log("[Consul] Initialising Game Engine...");

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
    glfwWindowHint(GLFW_RESIZABLE, true);

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
        console.Error("[Consul] Failed to create GLFW window");
    }

    glfwMakeContextCurrent(Window::handle);

    glfwSwapInterval(Window::vsyncEnabled ? 1 : 0);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) console.Error("[Consul] Failed to initialize GLAD");
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

    console.Log("[Consul] OpenGL initialised successfully");

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = NULL; // Disable saving .ini file
    io.LogFilename = NULL; // Disable logging to file
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;   // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;    // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;       // IF using Docking Branch

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(Window::handle, true);     // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();

    console.Log("[Consul] ImGui initialised successfully");

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

    // Start the ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    console.Draw("Console");

    ImGui::Render();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // ImGui::UpdatePlatformWindows();

    Window::SwapBuffers();

    return !Window::ShouldClose();
}

void Consul::Close()
{
    console.Log("[Consul] Shutting down Game Engine...");

    glfwDestroyWindow(Window::handle);
    glfwTerminate();

    console.Log("[Consul] GLFW terminated.");

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    console.Log("[Consul] ImGui terminated.");

    console.Log("[Consul] Shutdown complete.");
}