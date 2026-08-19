#include "consul.hpp"

#include <memory>
#include <utility>

#include "platforms/platform_glfw.hpp"
#include "graphics/renderer/opengl/opengl_renderer.hpp"
#include "imgui.h"
#include "implot.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"
#include "utils.hpp"
#include "core/profiling/profile_method.hpp"

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
    platform->initialiseGraphics(*renderer);
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

    engine.time.previousTime = platform->getTime();
    engine.time.currentTime = engine.time.previousTime;
    engine.time.deltaTime = engine.time.targetFrameTime;
}

void Consul::initialiseWindow(PlatformType platformType)
{
    switch (platformType) {
        case PlatformType::GLFW:
            platform = std::make_unique<PlatformGLFW>();
            break;
        default:
            console.error("[Consul] Unknown windowing platform!");
            break;
    }

    if (!platform) {
        console.error("[Consul] Failed to create windowing platform!");
        return;
    }

    platform->initialiseWindow();
}

void Consul::initialiseRenderer(GraphicsAPI gfxApi)
{
    switch (gfxApi) {
        case GraphicsAPI::OpenGL:
            renderer = std::make_unique<OpenGLRenderer>();
            break;
        default:
            console.error("[Consul] Unknown graphics API!");
            break;
    }
}

Consul::~Consul()
{
    console.log("[Consul] Shutting down Game Engine...");

    SceneManager* sceneManager = engine.getSceneManager();

    if (sceneManager) {
        sceneManager->shutdown();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
    console.log("[Consul] ImGui terminated.");

    renderer.reset();
    platform.reset();

    console.log("[Consul] Windowing platform terminated.");

    console.log("[Consul] Shutdown complete.");
}

void Consul::loadScene(std::unique_ptr<Scene> newScene)
{
    CONSUL_PROFILE_METHOD();

    SceneManager* sceneManager = engine.getSceneManager();

    if (!sceneManager) {
        console.error("[Consul::loadScene] Cannot load scene - scene manager is not initialised!");
        return;
    }

    if (sceneManager->hasScene()) {
        sceneManager->unloadScene();
    }
    renderer->releaseExpiredResources();
    sceneManager->loadScene(std::move(newScene));
}

void Consul::run()
{
    while (!close) {
        beginTick();
        Time& time = engine.time;
        engine.getSceneManager()->update();
        endTick();

        close = engine.window.shouldClose && platform->shouldClose();
    }
}

void Consul::beginTick()
{
    Time& time = engine.time;
    engine.profiler.beginFrame((float)(time.deltaTime));
    CONSUL_PROFILE_METHOD();

    time.currentTime = platform->getTime();
    time.renderTime = time.currentTime - time.previousTime;
    time.previousTime = time.currentTime;

    renderer->clearBackground(glm::vec4(0.f, 0.f, 0.f, 1.f));
    renderer->setViewport(0, 0, (int)engine.window.framebufferSize.x, (int)engine.window.framebufferSize.y);

    engine.inputSystem.beginTick();
    platform->pollEvents();
}

void Consul::endTick()
{
    CONSUL_PROFILE_METHOD();

    engine.window.shouldClose = platform->shouldClose();
    engine.inputSystem.endTick();

    Time& time = engine.time;

    time.currentTime = platform->getTime();
    time.updateTime = time.currentTime - time.previousTime;
    time.previousTime = time.currentTime;

    time.deltaTime = time.updateTime + time.renderTime;

    if (time.deltaTime < time.targetFrameTime) {
        time.previousTime = platform->getTime();
        waitTime(time.targetFrameTime - time.deltaTime);
        time.currentTime = platform->getTime();
        time.deltaTime += time.currentTime - time.previousTime;
    }
    time.previousTime = time.currentTime;
    time.frameCount++;

    // Rendering
    engine.getSceneManager()->render(*renderer);

    // Start the ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ui.update();

    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    platform->swapBuffers();

    renderer->releaseExpiredResources();

    engine.getMeshAssetManager()->removeExpiredAssets();
    engine.getModelAssetManager()->removeExpiredAssets();
    engine.getMaterialAssetManager()->removeExpiredAssets();
    engine.getTextureAssetManager()->removeExpiredAssets();
    engine.getShaderAssetManager()->removeExpiredAssets();
    engine.getVertexShaderAssetManager()->removeExpiredAssets();
    engine.getFragmentShaderAssetManager()->removeExpiredAssets();
}
