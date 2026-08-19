#pragma once

#include <memory>

#include "core/console/console.hpp"
#include "core/engine.hpp"
#include "core/ui/user_interface.hpp"
#include "core/project/asset_manager.hpp"
#include "core/project/scene.hpp"
#include "core/project/scene_manager.hpp"
#include "platforms/platform.hpp"
#include "graphics/renderer/renderer.hpp"

enum class PlatformType;
class AssetDefaults;
class GLTFImporter;

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
    Consul(Window& window) { engine->window = window; initialiseEngine(); };

    ~Consul();

    void initialiseEngine();

    /**
     * Runs the Consul application. Initialises a scene,
     * and updates the scene each frame.
     */
    void run();

    /**
     * Sets the target FPS.
     * @param fps Target frames per second.
     */
    void setTargetFPS(unsigned int fps)
    {
        if (fps <= 0) {
            return;
        }

        engine->time.targetFrameTime = 1.0 / (double)fps;
    }

    /**
     * Gets the renderer instance.
     * @returns Reference to the renderer.
    */
    Renderer& getRenderer() { return *renderer; }

    GLTFImporter& getGLTFImporter() { return *gltfImporter; }

    /**
     * Loads a scene. Ownership is transferred to the engine.
     * The previous scene (if any) will be shut down.
     */
    void loadScene(std::unique_ptr<Scene> newScene);

private:
    Console& console = Console::get();
    UserInterface ui;
    std::unique_ptr<Platform> platform;
    std::unique_ptr<Renderer> renderer;
    std::unique_ptr<Engine> engine = std::make_unique<Engine>();
    bool close = false;
    std::shared_ptr<AssetDefaults> assetDefaults = nullptr;
    std::shared_ptr<GLTFImporter> gltfImporter = nullptr;

    void initialiseWindow(PlatformType platformType);
    void initialiseRenderer(GraphicsAPI gfxApi);

    void beginTick();
    void endTick();
};
