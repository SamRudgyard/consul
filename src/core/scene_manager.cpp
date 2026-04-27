#include "core/scene_manager.hpp"

#include "console/console.hpp"
#include "core/profiling/profiler_scope.hpp"
#include "graphics/renderer/renderer.hpp"

void SceneManager::loadScene(std::unique_ptr<Scene> scene, Renderer& renderer)
{
    CONSUL_PROFILE_METHOD();

    // Close previous scene's resources
    if (currentScene) {
        renderer.clearSceneResources();
        if (currentScene->isInitialised) {
            currentScene->shutdown();
        }
    }

    currentScene = std::move(scene);
    if (!currentScene) {
        Console::get().error("[SceneManager::loadScene] Failed to load scene!");
        return;
    }

    currentScene->init(renderer);
}

void SceneManager::update(Renderer& renderer, double deltaTime)
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

    currentScene->update(deltaTime);
    currentScene->render(renderer);
}

void SceneManager::shutdown(Renderer& renderer)
{
    CONSUL_PROFILE_METHOD();

    renderer.clearSceneResources();

    if (!currentScene) {
        return;
    }

    if (currentScene->isInitialised) {
        currentScene->shutdown();
        currentScene->isInitialised = false;
    }
}
