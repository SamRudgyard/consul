#include "scene.hpp"
#include "graphics/renderer.hpp"

void Scene::init(Renderer& renderer)
{
    if (!isInitialised) {
        onInit(renderer);
        isInitialised = true;
    }
}

void Scene::update(float deltaTime)
{
    camera.handleInputs(deltaTime);
    onUpdate(deltaTime);
    updateNodes(deltaTime);
}

void Scene::render(Renderer& renderer)
{
    onRender(renderer);
    renderNodes(renderer);
}

void Scene::shutdown()
{
    isInitialised = false;
    onShutdown();
}