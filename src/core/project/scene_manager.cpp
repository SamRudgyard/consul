#include "core/project/scene_manager.hpp"

#include <algorithm>
#include <utility>
#include <vector>

#include "core/console/console.hpp"
#include "core/engine.hpp"
#include "core/ecs/components.hpp"
#include "core/profiling/profile_method.hpp"
#include "graphics/material/material.hpp"
#include "graphics/mesh/mesh.hpp"
#include "graphics/models/model.hpp"
#include "graphics/renderer/renderer.hpp"
#include "graphics/shader/shader.hpp"
#include "graphics/texture/texture.hpp"

void SceneManager::loadScene(std::unique_ptr<Scene> scene)
{
    CONSUL_PROFILE_METHOD();

    // Close previous scene's assets
    if (currentScene) {
        unloadScene();
    }

    currentScene = std::move(scene);
    if (!currentScene) {
        Console::get().error("[SceneManager::loadScene] Failed to load scene!");
        return;
    }

    currentScene->init();
}

void SceneManager::unloadScene()
{
    CONSUL_PROFILE_METHOD();

    if (!currentScene) {
        Console::get().error("[SceneManager::unloadScene] No scene is currently loaded!");
        return;
    }

    if (currentScene->isInitialised) {
        currentScene->shutdown();
    }

    currentScene.reset();
}

void SceneManager::update()
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

    currentScene->update();
}

void SceneManager::shutdown()
{
    CONSUL_PROFILE_METHOD();

    if (!currentScene) {
        return;
    }

    if (currentScene->isInitialised) {
        currentScene->shutdown();
    }

    currentScene.reset();
}
