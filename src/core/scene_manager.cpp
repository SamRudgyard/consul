#include "core/scene_manager.hpp"

#include "graphics/renderer.hpp"

void SceneManager::loadScene(std::unique_ptr<Scene> scene, Renderer& renderer)
{
    // Close previous scene's resources
    if (currentScene) {
        renderer.clearSceneResources();
    }

    currentScene = std::move(scene);
    assert(currentScene);

    currentScene->init(renderer);
    currentScene->isInitialised = true;
}

void SceneManager::update(Renderer& renderer, float deltaTime)
{
    assert(currentScene);
    assert(currentScene->isInitialised);

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
