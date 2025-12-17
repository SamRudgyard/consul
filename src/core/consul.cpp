#include "consul.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "platforms/platform_glfw.hpp"
#include "utils.hpp"

void Consul::initialiseEngine()
{
    // Prepare console
    console.clearLog();
    console.log("---- CONSUL ----");

    console.log("[Consul] Initialising Consul...");
    initialiseWindow(PlatformType::GLFW);
    console.log("[Consul] Windowing platform initialised.");

    initialiseRenderer(GraphicsAPI::OpenGL);
    console.log("[Consul] Graphics renderer initialised.");
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

    // Start the ImGui frame
    // ImGui_ImplOpenGL3_NewFrame();
    // ImGui_ImplGlfw_NewFrame();
    // ImGui::NewFrame();

    // console.draw("Console");

    // ImGui::Render();

    // ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // ImGui::UpdatePlatformWindows();

    return !(context->window.shouldClose && platform->shouldClose());
}

void Consul::beginTick()
{
    // platform->setMousePosition(context->inputSystem.getMousePosition().x, context->inputSystem.getMousePosition().y);

    platform->pollEvents();

    renderer->clearBackground(glm::vec4(0.f, 0.f, 0.f, 1.f));

    context->inputSystem.beginTick();
}

void Consul::endTick()
{
    Time& time = context->time;
    time.currentTime = platform->getTime();
    time.deltaTime = time.currentTime - time.previousTime;
    if (time.deltaTime < time.targetFrameTime) {
        sleep(time.targetFrameTime - time.deltaTime);
        time.currentTime = platform->getTime();
        time.deltaTime = time.currentTime - time.previousTime;
    }
    time.frameCount++;
    time.previousTime = time.currentTime;

    platform->swapBuffers();
    context->window.shouldClose = platform->shouldClose();
    
    context->inputSystem.endTick();
}

void Consul::terminate()
{
    console.log("[Consul] Shutting down Game Engine...");

    platform->terminate();
    console.log("[Consul] Windowing platform terminated.");

    // ImGui_ImplOpenGL3_Shutdown();
    // ImGui_ImplGlfw_Shutdown();
    // ImGui::DestroyContext();

    console.log("[Consul] ImGui terminated.");

    console.log("[Consul] Shutdown complete.");
}