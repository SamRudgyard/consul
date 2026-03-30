#include "scene.hpp"
#include "graphics/renderer/renderer.hpp"

void Scene::init(Renderer& renderer)
{
    if (!isInitialised) {
        onInit(renderer);
        isInitialised = true;
    }
}

void Scene::update(double deltaTime)
{
    onUpdate(deltaTime);
    updateNodes(deltaTime);
}

void Scene::render(Renderer& renderer)
{
    renderNodes(renderer);
    onRender(renderer);
}

void Scene::shutdown()
{
    isInitialised = false;
    onShutdown();
}