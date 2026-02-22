#pragma once

#include "core/node.hpp"
#include "graphics/camera/camera.hpp"

class Renderer;

class Scene
{
public:
    Scene() = default;
    ~Scene() { shutdown(); };

    Camera camera;
    bool isInitialised = false;

    void init(Renderer& renderer);
    void update(float deltaTime);
    void render(Renderer& renderer);
    void shutdown();

    Node& getRoot() { return root; }
    const Node& getRoot() const { return root; }

protected:
    virtual void onInit(Renderer& renderer) {} // User hook for scene-specific initialization logic
    virtual void onUpdate(float deltaTime) {} // User hook for scene-specific update logic
    virtual void onRender(Renderer& renderer) {} // User hook for scene-specific render logic
    virtual void onShutdown() {} // User hook for scene-specific shutdown logic

    void updateNodes(float deltaTime) { root.update(deltaTime, glm::mat4(1.0f)); }
    void renderNodes(Renderer& renderer) { root.render(renderer); }

private:
    Node root;
};
