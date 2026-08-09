#pragma once

#include "core/ecs/ecs.hpp"
#include "core/node.hpp"

class AssetLibrary;
class Camera;

class Scene
{
public:
    Scene() = default;
    virtual ~Scene() = default;

    bool isInitialised = false;

    void init(std::shared_ptr<AssetLibrary> assets);
    void update(std::shared_ptr<AssetLibrary> assets, double deltaTime);
    void shutdown();

    Node& getRoot() { return root; }
    const Node& getRoot() const { return root; }
    ECS& getECS() { return ecs; }
    const ECS& getECS() const { return ecs; }
    virtual Camera* getActiveCamera() { return nullptr; }

protected:
    virtual void onInit(std::shared_ptr<AssetLibrary> assets) {} // User hook for scene-specific initialization logic
    virtual void onUpdate(std::shared_ptr<AssetLibrary> assets, double deltaTime) {} // User hook for scene-specific update logic
    virtual void onShutdown() {} // User hook for scene-specific shutdown logic

    void updateNodes(std::shared_ptr<AssetLibrary> assets, double deltaTime) { root.update(assets, deltaTime, glm::mat4(1.0f)); }

private:
    Node root;
    ECS ecs;
};
