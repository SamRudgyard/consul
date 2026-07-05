#include "core/project/scene_manager.hpp"

#include "core/console/console.hpp"
#include "core/profiling/profile_method.hpp"
#include "core/project/asset_manager.hpp"
#include "graphics/camera/camera.hpp"
#include "graphics/mesh/mesh.hpp"
#include "graphics/models/model.hpp"
#include "graphics/renderer/renderer.hpp"
#include "graphics/shader/shader.hpp"

void SceneManager::loadScene(Scene& scene)
{
    CONSUL_PROFILE_METHOD();

    if (!assetManager) {
        Console::get().error("[SceneManager::loadScene] Cannot load scene - asset manager is not assigned!");
        return;
    }

    // Close previous scene's assets
    if (currentScene) {
        assetManager->clearAssets();

        if (currentScene->isInitialised) {
            currentScene->shutdown();
        }
    }

    currentScene = &scene;
    currentScene->init(assetManager);
}

void SceneManager::assignAssetManager(std::shared_ptr<AssetManager> assetManager)
{
    if (!assetManager) {
        Console::get().error("[SceneManager::assignAssetManager] Provided asset manager is null_ptr!");
        return;
    }

    this->assetManager = assetManager;
}

void SceneManager::update(double deltaTime)
{
    CONSUL_PROFILE_METHOD();

    if (!currentScene) {
        Console::get().error("[SceneManager::update] No scene was loaded!");
        return;
    }
    if (!currentScene->isInitialised) {
        Console::get().error("[SceneManager::update] Current scene is not initialised!");
        return;
    }

    currentScene->update(assetManager, deltaTime);
}

void SceneManager::render(Renderer& renderer)
{
    CONSUL_PROFILE_METHOD();

    if (!currentScene || !currentScene->isInitialised || !assetManager) {
        return;
    }

    Camera* camera = currentScene->getActiveCamera();
    if (!camera) {
        Console::get().error("[SceneManager::render] Current scene has no active camera!");
        return;
    }

    const auto& shaders = assetManager->getShaders();
    if (shaders.empty()) {
        Console::get().error("[SceneManager::render] Cannot render assets without a shader!");
        return;
    }

    for (const auto& [shaderID, shader] : shaders) {
        if (shader) {
            renderer.uploadShader(*shader);
        }
    }
    for (const auto& [modelID, model] : assetManager->getModels()) {
        if (model) {
            renderer.uploadModel(*model, *assetManager);
        }
    }
    for (const auto& [meshID, mesh] : assetManager->getMeshes()) {
        if (mesh) {
            renderer.uploadMesh(*mesh, *assetManager);
        }
    }

    renderer.render(*shaders.begin()->second, *camera, *assetManager);
}

void SceneManager::shutdown()
{
    CONSUL_PROFILE_METHOD();

    if (assetManager) {
        assetManager->clearAssets();
    }

    if (!currentScene) {
        return;
    }

    if (currentScene->isInitialised) {
        currentScene->shutdown();
        currentScene->isInitialised = false;
    }
}
