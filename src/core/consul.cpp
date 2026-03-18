#include "consul.hpp"

#include <memory>

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

    context->time.previousTime = platform->getTime();
    context->time.currentTime = context->time.previousTime;
    context->time.deltaTime = context->time.targetFrameTime;
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

void Consul::loadScene(std::unique_ptr<Scene> newScene)
{
    sceneManager.loadScene(std::move(newScene), *renderer);
}

void Consul::run()
{
    while (!close) {
        beginTick();
        Time& time = context->time;
        sceneManager.update(*renderer, time.deltaTime);
        context->fpsMonitor.update(time.deltaTime);
        endTick();

        close = context->window.shouldClose && platform->shouldClose();
    }
}

void Consul::beginTick()
{
    Time& time = context->time;
    time.currentTime = platform->getTime();
    time.renderTime = time.currentTime - time.previousTime;
    time.previousTime = time.currentTime;

    renderer->clearBackground(glm::vec4(0.f, 0.f, 0.f, 1.f));
    renderer->setViewport(0, 0, (int)context->window.framebufferSize.x, (int)context->window.framebufferSize.y);
    
    context->inputSystem.beginTick();
}

void Consul::endTick()
{

    context->window.shouldClose = platform->shouldClose();
    context->inputSystem.endTick();

    platform->pollEvents();

    Time& time = context->time;

    time.currentTime = platform->getTime();
    time.updateTime = time.currentTime - time.previousTime;
    time.previousTime = time.currentTime;

    time.deltaTime = time.updateTime + time.renderTime;

    Console& console = Console::get();

    if (time.deltaTime < time.targetFrameTime) {
        time.previousTime = platform->getTime();
        waitTime(time.targetFrameTime - time.deltaTime);
        time.currentTime = platform->getTime();
        time.deltaTime += time.currentTime - time.previousTime;
    }

    time.frameCount++;

    // Rendering

    // Start the ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    context->ui.render();

    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    platform->swapBuffers();
}

void Consul::terminate()
{
    console.log("[Consul] Shutting down Game Engine...");
    context->ui.unregisterWindow("Console");
    context->ui.unregisterWindow("FPS Monitor");

    sceneManager.shutdown(*renderer);

    ImGui_ImplGlfw_Shutdown();

    if (platform) {
        platform->terminate();
    }

    console.log("[Consul] Windowing platform terminated.");

    ImGui_ImplOpenGL3_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    console.log("[Consul] ImGui terminated.");

    console.log("[Consul] Shutdown complete.");
}
