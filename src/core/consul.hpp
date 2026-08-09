#pragma once

#include "core/console/console.hpp"
#include "core/engine.hpp"
#include "core/ecs/ecs.hpp"
#include "core/ui/user_interface.hpp"
#include "core/project/asset_manager.hpp"
#include "core/project/asset_library.hpp"
#include "core/project/scene.hpp"
#include "core/project/scene_manager.hpp"
#include "platforms/platform.hpp"
#include "graphics/renderer/renderer.hpp"

#include <memory>

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
    Consul(Window& window) { context->window = window; initialiseEngine(); };

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

        context->time.targetFrameTime = 1.0 / (double)fps;
    }

    /**
     * Gets the renderer instance.
     * @returns Reference to the renderer.
    */
    Renderer& getRenderer() { return *renderer; }

    /**
     * Gets the scene-facing asset library.
     * @returns Reference to the asset library.
     */
    AssetLibrary& getAssets() { return *assets; }
    AssetLibrary& getAssetManager() { return *assets; }

    ModelAssetManager& getModelAssetManager() { return *modelAssets; }
    MeshAssetManager& getMeshAssetManager() { return *meshAssets; }
    MaterialAssetManager& getMaterialAssetManager() { return *materialAssets; }
    TextureAssetManager& getTextureAssetManager() { return *textureAssets; }
    ShaderAssetManager& getShaderAssetManager() { return *shaderAssets; }
    VertexShaderAssetManager& getVertexShaderAssetManager() { return *vertexShaderAssets; }
    FragmentShaderAssetManager& getFragmentShaderAssetManager() { return *fragmentShaderAssets; }
    GLTFImporter& getGLTFImporter() { return *gltfImporter; }
    ECS& getECS() { return ecs; }

    /**
     * Gets the scene manager instance.
     * @returns Reference to the scene manager.
     */
    SceneManager& getSceneManager() { return *sceneManager; }

    /**
     * Loads a scene. Ownership is transferred to the engine.
     * The previous scene (if any) will be shut down.
     */
    void loadScene(std::unique_ptr<Scene> newScene);

private:
    Console& console = Console::get();
    Engine* context = Engine::get();
    UserInterface ui;
    std::unique_ptr<Platform> platform;
    std::unique_ptr<Renderer> renderer;
    bool close = false;
    std::shared_ptr<ModelAssetManager> modelAssets = nullptr;
    std::shared_ptr<MeshAssetManager> meshAssets = nullptr;
    std::shared_ptr<MaterialAssetManager> materialAssets = nullptr;
    std::shared_ptr<TextureAssetManager> textureAssets = nullptr;
    std::shared_ptr<ShaderAssetManager> shaderAssets = nullptr;
    std::shared_ptr<VertexShaderAssetManager> vertexShaderAssets = nullptr;
    std::shared_ptr<FragmentShaderAssetManager> fragmentShaderAssets = nullptr;
    std::shared_ptr<AssetDefaults> assetDefaults = nullptr;
    std::shared_ptr<GLTFImporter> gltfImporter = nullptr;
    std::shared_ptr<AssetLibrary> assets = nullptr;
    std::shared_ptr<SceneManager> sceneManager = nullptr;
    ECS ecs;

    void initialiseWindow(PlatformType platformType);
    void initialiseRenderer(GraphicsAPI gfxApi);

    void beginTick();
    void endTick();
};
