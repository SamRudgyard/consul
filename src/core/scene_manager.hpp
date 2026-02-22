#pragma once

#include <memory>

#include "core/scene.hpp"

class Renderer;

class SceneManager
{
public:
    SceneManager() = default;
    ~SceneManager() = default;

    void loadScene(std::unique_ptr<Scene> newScene, Renderer& renderer);

    void update(Renderer& renderer, float deltaTime);
    void shutdown(Renderer& renderer);

private:
    std::unique_ptr<Scene> currentScene;
};
