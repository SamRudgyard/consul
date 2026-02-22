#include "core/scene_manager.hpp"

#include "graphics/renderer.hpp"

void SceneManager::loadScene(std::unique_ptr<Scene> scene, Renderer& renderer)
{
    // Close previous scene
    if (currentScene && currentScene->isInitialised) {
        currentScene->shutdown();
        currentScene->isInitialised = false;
        renderer.clearSceneResources();
    }

    currentScene = std::move(scene);
    assert(currentScene);

    currentScene->initialise(renderer);
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
    assert(currentScene);

    if (currentScene->isInitialised) {
        currentScene->shutdown();
        currentScene->isInitialised = false;
    }

    renderer.clearSceneResources();
}
