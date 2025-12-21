#include "consul.hpp"

#include "platforms/platform_glfw.hpp"
#include "imgui.h"
#include "implot.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"
#include "utils.hpp"

void Consul::initialiseEngine()
{
    // Prepare console
    console.clearLog();
    console.log("---- CONSUL ----");

    console.log("[Consul] Initialising Consul...");
    initialiseWindow(PlatformType::GLFW);
    console.log("[Consul] Windowing platform initialised.");

    GraphicsAPI gfxApi = GraphicsAPI::OpenGL;
    initialiseRenderer(gfxApi);
    console.log("[Consul] Graphics renderer initialised.");

    // Setup ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;   // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;    // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange; // Don't let ImGui move the mouse
    ImGui::StyleColorsDark();
    platform->initialiseImGui(gfxApi);
    renderer->initialiseImGui();
    console.log("[Consul] ImGui initialised.");

    context->ui.registerWindow("Console", [this](const std::string& name, bool* open) { console.draw(name, open); }, &consoleWindowOpen);
    context->ui.registerWindow("FPS Monitor", [this](const std::string& name, bool* open) { context->fpsMonitor.draw(name, open); }, &fpsMonitorWindowOpen);
}

void Consul::initialiseWindow(PlatformType platformType)
{
    switch (platformType) {
        case PlatformType::GLFW:
            platform = new PlatformGLFW();
            break;
        default:
            console.error("[Consul] Unknown windowing platform!");
            break;
    }

    if (platform) {
        platform->initialiseWindow();
    }
}

void Consul::initialiseRenderer(GraphicsAPI gfxApi)
{
    renderer = new Renderer(platform, gfxApi);
}

Consul::~Consul()
{
    terminate();
}

bool Consul::run()
{
    endTick();
    beginTick();

    if (context->window.shouldClose) {
        return false;
    }

    return !(context->window.shouldClose && platform->shouldClose());
}

void Consul::beginTick()
{
    platform->pollEvents();

    renderer->clearBackground(glm::vec4(0.f, 0.f, 0.f, 1.f));
    renderer->setViewport(0, 0, (int)context->window.framebufferSize.x, (int)context->window.framebufferSize.y);

    context->inputSystem.beginTick();
}

void Consul::endTick()
{
    Time& time = context->time;
    time.currentTime = platform->getTime();
    time.deltaTime = time.currentTime - time.previousTime;
    if (time.deltaTime < time.targetFrameTime) {
        waitTime(time.targetFrameTime - time.deltaTime);
    }
    time.currentTime = platform->getTime();
    time.deltaTime = time.currentTime - time.previousTime;
    time.frameCount++;
    time.previousTime = time.currentTime;

    // Start the ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    context->fpsMonitor.update(context->time.deltaTime);
    context->ui.render();

    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // ImGui::UpdatePlatformWindows();

    platform->swapBuffers();
    context->window.shouldClose = platform->shouldClose();
    
    context->inputSystem.endTick();
}

void Consul::terminate()
{
    console.log("[Consul] Shutting down Game Engine...");
    context->ui.unregisterWindow("Console");
    context->ui.unregisterWindow("FPS Monitor");

    ImGui_ImplGlfw_Shutdown();
    platform->terminate();
    console.log("[Consul] Windowing platform terminated.");

    ImGui_ImplOpenGL3_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    console.log("[Consul] ImGui terminated.");

    console.log("[Consul] Shutdown complete.");
}
