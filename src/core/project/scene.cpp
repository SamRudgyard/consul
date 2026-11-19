#include "scene.hpp"

#include "core/console/console.hpp"
#include "core/profiling/profile_method.hpp"

void Scene::init()
{
    CREMA_PROFILE_METHOD();

    if (!isInitialised) {
        onInit();
        isInitialised = true;
    }
}

void Scene::update()
{
    CREMA_PROFILE_METHOD();

    onUpdate();
}

void Scene::shutdown()
{
    CREMA_PROFILE_METHOD();

    if (!isInitialised) {
        Console::get().error("[Scene::shutdown] Scene is not initialised!");
        return;
    }

    isInitialised = false;
    onShutdown();
}
