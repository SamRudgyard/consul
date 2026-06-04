#pragma once

#include <memory>

#include "core/project/scene.hpp"

class AssetManager;
class Renderer;

class SceneManager
{
public:
    SceneManager() = default;
    ~SceneManager() = default;

    void loadScene(Scene& newScene);

    void assignAssetManager(std::shared_ptr<AssetManager> assetManager);

    void update(double deltaTime);
    void render(Renderer& renderer);
    void shutdown();

private:
    Scene* currentScene = nullptr;
    std::shared_ptr<AssetManager> assetManager;
};
