#include "scene.hpp"
#include "core/console/console.hpp"
#include "core/profiling/profile_method.hpp"
#include "core/project/asset_manager.hpp"

void Scene::init(std::shared_ptr<AssetManager> assetManager)
{
    CONSUL_PROFILE_METHOD();

    if (!assetManager) {
        Console::get().error("[Scene::init] Provided asset manager is null_ptr!");
        return;
    }

    if (!isInitialised) {
        onInit(assetManager);
        isInitialised = true;
    }
}

void Scene::update(std::shared_ptr<AssetManager> assetManager, double deltaTime)
{
    CONSUL_PROFILE_METHOD();

    onUpdate(assetManager, deltaTime);
    updateNodes(assetManager, deltaTime);
}

void Scene::shutdown()
{
    CONSUL_PROFILE_METHOD();

    if (!isInitialised) {
        Console::get().error("[Scene::shutdown] Scene is not initialised!");
        return;
    }

    isInitialised = false;
    onShutdown();
}
