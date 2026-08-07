#pragma once

#include <memory>

#include "core/project/scene.hpp"

class AssetLibrary;
class Renderer;

class SceneManager
{
public:
    SceneManager() = default;
    ~SceneManager() = default;

    void loadScene(std::unique_ptr<Scene> newScene);

    void assignAssets(std::shared_ptr<AssetLibrary> assets);

    void update(double deltaTime);
    void render(Renderer& renderer);
    void shutdown();

private:
    std::unique_ptr<Scene> currentScene;
    std::shared_ptr<AssetLibrary> assets;
};
