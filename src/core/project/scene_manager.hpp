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

    void loadScene(std::unique_ptr<Scene> newScene);
    void unloadScene();
    bool hasScene() const { return currentScene != nullptr; }
    const Scene& getCurrentScene() const { return *currentScene; }


    void update();
    void shutdown();

private:
    std::unique_ptr<Scene> currentScene;
};
