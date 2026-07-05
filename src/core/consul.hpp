#pragma once

#include "core/console/console.hpp"
#include "core/engine_context.hpp"
#include "core/ui/user_interface.hpp"
#include "core/project/asset_manager.hpp"
#include "core/project/scene.hpp"
#include "core/project/scene_manager.hpp"
#include "platforms/platform.hpp"
#include "graphics/renderer/renderer.hpp"
#include "time.hpp"

#include <memory>

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
     * Gets the asset manager instance.
     * @returns Reference to the asset manager.
     */
    AssetManager& getAssetManager() { return *assetManager; }

    /**
     * Gets the scene manager instance.
     * @returns Reference to the scene manager.
     */
    SceneManager& getSceneManager() { return *sceneManager; }

    /**
     * Loads a scene.
     * The previous scene (if any) will be shut down.
     */
    void loadScene(Scene& newScene);

private:
    Console& console = Console::get();
    EngineContext* context = EngineContext::get();
    UserInterface ui;
    Platform* platform = nullptr;
    Renderer* renderer = nullptr;
    bool close = false;
    std::shared_ptr<AssetManager> assetManager = nullptr;
    std::shared_ptr<SceneManager> sceneManager = nullptr;

    void initialiseWindow(PlatformType platformType);
    void initialiseRenderer(GraphicsAPI gfxApi);

    void beginTick();
    void endTick();
};
