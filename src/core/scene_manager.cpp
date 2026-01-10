#include "core/scene_manager.hpp"

#include "graphics/renderer.hpp"

void SceneManager::loadScene(Scene* newScene, Renderer& renderer)
{
    if (currentScene == newScene) {
        return;
    }

    if (currentScene && currentScene->isInitialised) {
        currentScene->shutdown();
        currentScene->isInitialised = false;
    }

    currentScene = newScene;

    if (currentScene) {
        currentScene->isInitialised = false;
        currentScene->initialise(renderer);
        currentScene->isInitialised = true;
    }
}

void SceneManager::update(Renderer& renderer, float deltaTime)
{
    if (!currentScene) {
        return;
    }

    if (!currentScene->isInitialised) {
        currentScene->initialise(renderer);
        currentScene->isInitialised = true;
    }

    currentScene->update(deltaTime);
    currentScene->render(renderer);
}

void SceneManager::shutdown()
{
    if (currentScene && currentScene->isInitialised) {
        currentScene->shutdown();
        currentScene->isInitialised = false;
    }

    currentScene = nullptr;
}
