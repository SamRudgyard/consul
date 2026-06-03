#pragma once

#include "core/project/asset_manager.hpp"
#include "core/project/scene_manager.hpp"

#include <memory>

class ProjectManager
{
public:
    ProjectManager() = default;
    ~ProjectManager() = default;

    /**
     * Gets the asset manager instance.
     * @returns Reference to the asset manager.
     */
    std::shared_ptr<AssetManager> getAssetManager() const { return assetManager; }

    /**
     * Gets the scene manager instance.
     * @returns Reference to the scene manager.
     */
    std::shared_ptr<SceneManager> getSceneManager() const { return sceneManager; }

private:
    std::shared_ptr<AssetManager> assetManager = std::make_shared<AssetManager>();
    std::shared_ptr<SceneManager> sceneManager = std::make_shared<SceneManager>();
};
