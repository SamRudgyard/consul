#include "scene.hpp"
#include "core/console/console.hpp"
#include "core/profiling/profile_method.hpp"

void Scene::init()
{
    CONSUL_PROFILE_METHOD();

    if (!isInitialised) {
        onInit();
        isInitialised = true;
    }
}

void Scene::update()
{
    CONSUL_PROFILE_METHOD();

    onUpdate();
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
