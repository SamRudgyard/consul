#include "scene.hpp"
#include "graphics/renderer.hpp"

void Scene::update(float deltaTime)
{
    camera.handleInputs(deltaTime);
    updateSceneGraph(deltaTime);
}