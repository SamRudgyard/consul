#include "core/project/scene_manager.hpp"

#include "core/console/console.hpp"
#include "core/profiling/profile_method.hpp"
#include "core/project/asset_library.hpp"
#include "graphics/camera/camera.hpp"
#include "graphics/mesh/mesh.hpp"
#include "graphics/models/model.hpp"
#include "graphics/renderer/renderer.hpp"
#include "graphics/shader/shader.hpp"

void SceneManager::loadScene(Scene& scene)
{
    CONSUL_PROFILE_METHOD();

    if (!assets) {
        Console::get().error("[SceneManager::loadScene] Cannot load scene - assets are not assigned!");
        return;
    }

    // Close previous scene's assets
    if (currentScene) {
        assets->clearAssets();

        if (currentScene->isInitialised) {
            currentScene->shutdown();
        }
    }

    currentScene = &scene;
    currentScene->init(assets);
}

void SceneManager::assignAssets(std::shared_ptr<AssetLibrary> assets)
{
    if (!assets) {
        Console::get().error("[SceneManager::assignAssets] Provided assets are null_ptr!");
        return;
    }

    this->assets = assets;
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

    currentScene->update(assets, deltaTime);
}

void SceneManager::render(Renderer& renderer)
{
    CONSUL_PROFILE_METHOD();

    if (!currentScene || !currentScene->isInitialised || !assets) {
        return;
    }

    Camera* camera = currentScene->getActiveCamera();
    if (!camera) {
        Console::get().error("[SceneManager::render] Current scene has no active camera!");
        return;
    }

    const auto& shaders = assets->getShaders();
    if (shaders.empty()) {
        Console::get().error("[SceneManager::render] Cannot render assets without a shader!");
        return;
    }

    for (const auto& [shaderID, shader] : shaders) {
        if (shader) {
            renderer.uploadShader(*shader);
        }
    }
    for (const auto& [modelID, model] : assets->getModels()) {
        if (model) {
            renderer.uploadModel(*model, *assets);
        }
    }
    for (const auto& [meshID, mesh] : assets->getMeshes()) {
        if (mesh) {
            renderer.uploadMesh(*mesh, *assets);
        }
    }

    renderer.render(*shaders.begin()->second, *camera, *assets);
}

void SceneManager::shutdown()
{
    CONSUL_PROFILE_METHOD();

    if (assets) {
        assets->clearAssets();
    }

    if (!currentScene) {
        return;
    }

    if (currentScene->isInitialised) {
        currentScene->shutdown();
        currentScene->isInitialised = false;
    }
}
