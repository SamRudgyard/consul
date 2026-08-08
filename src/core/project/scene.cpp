#include "scene.hpp"
#include "core/console/console.hpp"
#include "core/profiling/profile_method.hpp"
#include "core/project/asset_library.hpp"

void Scene::init(std::shared_ptr<AssetLibrary> assets)
{
    CONSUL_PROFILE_METHOD();

    if (!assets) {
        Console::get().error("[Scene::init] Provided asset library is null_ptr!");
        return;
    }

    if (!isInitialised) {
        onInit(assets);
        isInitialised = true;
    }
}

void Scene::update(std::shared_ptr<AssetLibrary> assets, double deltaTime)
{
    CONSUL_PROFILE_METHOD();

    onUpdate(assets, deltaTime);
    updateNodes(assets, deltaTime);
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
