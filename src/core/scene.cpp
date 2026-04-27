#include "scene.hpp"
#include "core/profiling/profiler_scope.hpp"
#include "graphics/renderer/renderer.hpp"

void Scene::init(Renderer& renderer)
{
    CONSUL_PROFILE_METHOD();

    if (!isInitialised) {
        onInit(renderer);
        isInitialised = true;
    }
}

void Scene::update(double deltaTime)
{
    CONSUL_PROFILE_METHOD();

    onUpdate(deltaTime);
    updateNodes(deltaTime);
}

void Scene::render(Renderer& renderer)
{
    CONSUL_PROFILE_METHOD();

    renderNodes(renderer);
    onRender(renderer);
}

void Scene::shutdown()
{
    CONSUL_PROFILE_METHOD();

    isInitialised = false;
    onShutdown();
}
