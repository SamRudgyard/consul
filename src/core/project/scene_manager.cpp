#include "core/project/scene_manager.hpp"

#include <utility>
#include <vector>

#include "core/console/console.hpp"
#include "core/profiling/profile_method.hpp"
#include "core/project/asset_library.hpp"
#include "graphics/camera/camera.hpp"
#include "graphics/material/material.hpp"
#include "graphics/mesh/mesh.hpp"
#include "graphics/models/model.hpp"
#include "graphics/renderer/renderer.hpp"
#include "graphics/shader/shader.hpp"
#include "graphics/texture/texture.hpp"

void SceneManager::loadScene(std::unique_ptr<Scene> scene)
{
    CONSUL_PROFILE_METHOD();

    if (!assets) {
        Console::get().error("[SceneManager::loadScene] Cannot load scene - assets are not assigned!");
        return;
    }

    // Close previous scene's assets
    if (currentScene) {
        unloadScene();
    }

    currentScene = std::move(scene);
    if (!currentScene) {
        Console::get().error("[SceneManager::loadScene] Failed to load scene!");
        return;
    }

    currentScene->init(assets);
}

void SceneManager::unloadScene()
{
    CONSUL_PROFILE_METHOD();

    if (!currentScene) {
        Console::get().error("[SceneManager::unloadScene] No scene is currently loaded!");
        return;
    }

    if  (assets) {
        assets->clearAssets();
    }

    if (currentScene->isInitialised) {
        currentScene->shutdown();
    }

    currentScene.reset();
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

    auto uploadMesh = [this, &renderer](AssetID meshID, Mesh& mesh) {
        renderer.uploadMesh(meshID, mesh);

        std::shared_ptr<Material> material = mesh.getMaterial();
        if (!material) {
            return;
        }

        std::shared_ptr<Texture> albedoTexture = material->getAlbedoTexture();
        std::shared_ptr<Texture> specularTexture = material->getSpecularTexture();
        std::shared_ptr<Texture> normalTexture = material->getNormalTexture();
        if (albedoTexture) renderer.uploadTexture(albedoTexture);
        if (specularTexture) renderer.uploadTexture(specularTexture);
        if (normalTexture) renderer.uploadTexture(normalTexture);
    };

    for (const auto& [shaderID, shader] : shaders) {
        if (!shader) {
            continue;
        }

        std::shared_ptr<VertexShader> vertexShader = shader->getVertexShader();
        std::shared_ptr<FragmentShader> fragmentShader = shader->getFragmentShader();
        if (!vertexShader || !fragmentShader) {
            Console::get().error("[SceneManager::render] Shader references a missing vertex or fragment shader asset.");
            continue;
        }

        renderer.uploadShader(shaderID, *vertexShader, *fragmentShader);
    }

    for (const auto& [modelID, model] : assets->getModels()) {
        if (!model) {
            continue;
        }

        const std::vector<AssetID>& meshIDs = model->getMeshIDs();
        std::vector<glm::mat4> transforms = model->getTransformationMatrices();
        for (unsigned int iMesh = 0; iMesh < meshIDs.size(); iMesh++) {
            std::shared_ptr<Mesh> mesh = assets->getMesh(meshIDs[iMesh]);
            if (!mesh) {
                continue;
            }

            mesh->setModelMatrix(transforms[iMesh]);
            uploadMesh(meshIDs[iMesh], *mesh);
        }
    }
    
    for (const auto& [meshID, mesh] : assets->getMeshes()) {
        if (mesh) {
            uploadMesh(meshID, *mesh);
        }
    }

    renderer.render(shaders.begin()->first, *camera, *assets);
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
