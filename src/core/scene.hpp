#pragma once

#include "core/node.hpp"

class Renderer;

class Scene
{
public:
    virtual ~Scene() = default;

    bool isInitialised = false;

    virtual void initialise(Renderer& renderer) = 0;
    virtual void update(float deltaTime) = 0;
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
