#include "core/scene_manager.hpp"

#include "console/console.hpp"
#include "graphics/renderer/renderer.hpp"

void SceneManager::loadScene(std::unique_ptr<Scene> scene, Renderer& renderer)
{
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
    renderer.clearSceneResources();

    if (!currentScene) {
        return;
    }

    if (currentScene->isInitialised) {
        currentScene->shutdown();
        currentScene->isInitialised = false;
    }
}
