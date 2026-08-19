#include "scene.hpp"
#include "core/console/console.hpp"
#include "core/profiling/profile_method.hpp"

void Scene::init(Engine& engine)
{
    CONSUL_PROFILE_METHOD();

    if (!isInitialised) {
        onInit(engine);
        isInitialised = true;
    }
}

void Scene::update(Engine& engine, double deltaTime)
{
    CONSUL_PROFILE_METHOD();

    onUpdate(engine, deltaTime);
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
