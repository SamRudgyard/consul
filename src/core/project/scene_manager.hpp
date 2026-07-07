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

    void loadScene(Scene& newScene);

    void assignAssets(std::shared_ptr<AssetLibrary> assets);

    void update(double deltaTime);
    void render(Renderer& renderer);
    void shutdown();

private:
    Scene* currentScene = nullptr;
    std::shared_ptr<AssetLibrary> assets;
};
