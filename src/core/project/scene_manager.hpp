#pragma once

#include <memory>

#include "core/project/scene.hpp"

class Engine;
class Renderer;

class SceneManager
{
public:
    SceneManager() = default;
    ~SceneManager() = default;

    void loadScene(std::unique_ptr<Scene> newScene, Engine& engine);
    void unloadScene();
    bool hasScene() const { return currentScene != nullptr; }

    void update(Engine& engine, double deltaTime);
    void render(Engine& engine, Renderer& renderer);
    void shutdown();

private:
    std::unique_ptr<Scene> currentScene;
};
