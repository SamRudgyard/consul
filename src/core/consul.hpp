#pragma once

#include "core/console/console.hpp"
#include "core/engine_context.hpp"
#include "core/scene.hpp"
#include "core/scene_manager.hpp"
#include "platforms/platform.hpp"
#include "graphics/renderer.hpp"
#include "time.hpp"

enum class PlatformType;

class Consul
{
public:

    /**
     * Initialise a Consul application, using default window configuration.
     */
    Consul() { initialiseEngine(); };

    /**
     * Initialise a Consul application with the given window configuration.
     * @param window Window configuration settings.
     */
    Consul(Window& window) { context->window = window; initialiseEngine(); };

    ~Consul();

    void initialiseEngine();

    /**
     * Runs the Consul application. Initialises a scene,
     * and updates the scene each frame.
     */
    void run();

    /**
     * Gracefully terminate the Consul application.
     */
    void terminate();

    /**
     * Sets the target FPS.
     * @param fps Target frames per second.
     */
    void setTargetFPS(unsigned int fps)
    {
        if (fps <= 0) {
            return;
        }

        context->time.targetFrameTime = 1.0 / (double)fps;
    }

    /**
     * Gets the renderer instance.
     * @returns Reference to the renderer.
    */
    Renderer& getRenderer() { return *renderer; }

    /**
     * Loads a scene. The previous scene (if any) will be shut down.
     */
    void loadScene(Scene* newScene);

private:
    Console& console = Console::get();
    EngineContext* context = EngineContext::get();
    IPlatform* platform = nullptr;
    Renderer* renderer = nullptr;
    bool consoleWindowOpen = true;
    bool fpsMonitorWindowOpen = true;
    bool close = false;
    SceneManager sceneManager;

    void initialiseWindow(PlatformType platformType);
    void initialiseRenderer(GraphicsAPI gfxApi);

    void beginTick();
    void endTick();
};
