#include "consul.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "utils.hpp"

Consul::Consul(const char* title, bool isFullscreen)
    : Consul(title, 0, 0, isFullscreen)
{
}

Consul::Consul(const char* title, unsigned int width, unsigned int height, bool isFullscreen)
{
    // Prepare console
    console.clearLog();
    console.log("---- CONSUL ----");

    console.log("[Consul] Initialising Consul...");

    if (!glfwInit()) {
        console.error("[Consul] Failed to initialize GLFW");
        return;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_RESIZABLE, true);

    GLFWmonitor* primaryMonitor = nullptr;
    if (isFullscreen) {
        console.log("[Consul] Creating fullscreen window...");
        primaryMonitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
        width = mode->width;
        height = mode->height;
    }

    bool maximise = !isFullscreen && (width == 0 || height == 0);

    Window::title = title;
    Window::width = maximise ? 1280 : width;
    Window::height = maximise ? 720 : height;
    Window::isFullscreen = isFullscreen;
    Window::handle = glfwCreateWindow(Window::width, Window::height, title, primaryMonitor, nullptr);

    if (maximise) {
        console.log("[Consul] Creating maximised window...");
        glfwMaximizeWindow(Window::handle);

        int framebufferWidth, framebufferHeight;
        glfwGetFramebufferSize(Window::handle, &framebufferWidth, &framebufferHeight);
        Window::width = static_cast<unsigned int>(framebufferWidth);
        Window::height = static_cast<unsigned int>(framebufferHeight);
    }

    if (!Window::handle) {
        glfwTerminate();
        console.error("[Consul] Failed to create GLFW window");
        return;
    }

    glfwMakeContextCurrent(Window::handle);

    glfwSwapInterval(Window::vsyncEnabled ? 1 : 0);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) console.error("[Consul] Failed to initialize GLAD");
    glfwSetKeyCallback(Window::handle, Keyboard::keyCallback);
    glfwSetWindowSizeCallback(Window::handle, Window::windowSizeCallback);
    // glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    const GLubyte* version = glGetString(GL_VERSION);
    if (!version) {
        Console::get().error("[Consul] [OpenGL] Failed to retrieve OpenGL version");
    } else {
        Console::get().log("[Consul] [OpenGL] Found OpenGL version " + std::string(reinterpret_cast<const char*>(version)));
    }

    // Initialise OpenGL with our default settings
    // --------------------
    glEnable(GL_DEPTH_TEST);                                // Enable depth testing
    glDepthFunc(GL_LESS);                                   // Type of depth testing to apply
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);      // Colour blending, determines how pixel colours are combined
    glEnable(GL_BLEND);                                     // Enable colour blending (required for transparencies)
    glCullFace(GL_BACK);                                    // Cull back faces
    glFrontFace(GL_CCW);                                    // Front faces are counter clockwise
    glEnable(GL_CULL_FACE);                                 // Enable backface culling
    // glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);                 // Enable seamless cubemap texture

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);                   // Set clear colour to black
    glClearDepth(1.0f);                                     // Set clear depth to farthest possible depth when glClear(GL_DEPTH_BUFFER_BIT) is called
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // Clear colour and depth buffers

    glCheckError();
    Window::setupViewport(Window::width, Window::height);

    console.log("[Consul] OpenGL initialised successfully");

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = NULL; // Disable saving .ini file
    io.LogFilename = NULL; // Disable logging to file
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;   // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;    // Enable Gamepad Controls

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(Window::handle, true);     // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();

    console.log("[Consul] ImGui initialised successfully");

    Time::frameCount = 0;
    Window::toClose = false;
}

Consul::~Consul()
{
    terminate();
}

void Consul::setVSync(bool enabled)
{
    Window::setVSync(enabled);
}

bool Consul::run()
{
    Time::newFrame();

    // Start the ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    console.draw("Console");

    ImGui::Render();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // ImGui::UpdatePlatformWindows();

    return !Window::shouldClose();
}

void Consul::terminate()
{
    console.log("[Consul] Shutting down Game Engine...");

    glfwDestroyWindow(Window::handle);
    glfwTerminate();

    console.log("[Consul] GLFW terminated.");

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    console.log("[Consul] ImGui terminated.");

    console.log("[Consul] Shutdown complete.");
}