#pragma once

#include "core/scene.hpp"

class Renderer;

class SceneManager
{
public:
    SceneManager() = default;
    ~SceneManager() = default;

    void loadScene(Scene* newScene, Renderer& renderer);

    void update(Renderer& renderer, float deltaTime);
    void shutdown();

    Scene* getScene() const { return currentScene; }

private:
    Scene* currentScene = nullptr;
};
