#pragma once

#include "core/node.hpp"
#include "graphics/camera/camera.hpp"

class Renderer;

class Scene
{
public:
    Scene() = default;
    virtual ~Scene() = default;

    Camera camera;
    bool isInitialised = false;

    virtual void initialise(Renderer& renderer) = 0;
    void update(float deltaTime);
    virtual void render(Renderer& renderer) = 0;
    virtual void shutdown() = 0;

    Node& getRoot() { return root; }
    const Node& getRoot() const { return root; }

protected:
    void updateSceneGraph(float deltaTime) { root.update(deltaTime, glm::mat4(1.0f)); }
    void renderSceneGraph(Renderer& renderer) { root.render(renderer); }

private:
    Node root;
};
