#include "consul.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "platforms/platform_glfw.hpp"
#include "utils.hpp"

Consul::Consul()
{
    // Prepare console
    console.clearLog();
    console.log("---- CONSUL ----");

    console.log("[Consul] Initialising Consul...");
    initialiseWindow(PlatformType::GLFW);
    console.log("[Consul] Windowing platform initialised.");

    initialiseGraphics(GraphicsAPI::OpenGL);
    console.log("[Consul] Graphics renderer initialised.");
}

void Consul::initialiseWindow(PlatformType platformType)
{
    switch (platformType) {
        case PlatformType::GLFW:
            platform = new PlatformGLFW(&windowConfig);
            break;
        default:
            console.error("[Consul] Unknown windowing platform!");
            break;
    }

    if (platform) {
        platform->initialiseWindow();
    }
}

void Consul::initialiseGraphics(GraphicsAPI gfxApi)
{
    renderer = new Renderer(gfxApi);
}

Consul::~Consul()
{
    terminate();
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

    renderer->clearBackground(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // ImGui::UpdatePlatformWindows();

    return !(windowConfig.shouldClose && platform->shouldClose());
}

void Consul::terminate()
{
    console.log("[Consul] Shutting down Game Engine...");

    platform->terminate();
    delete platform;

    console.log("[Consul] Windowing platform terminated.");

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    console.log("[Consul] ImGui terminated.");

    console.log("[Consul] Shutdown complete.");
}