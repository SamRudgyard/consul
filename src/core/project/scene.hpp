#pragma once

#include "core/node.hpp"

class AssetManager;
class Camera;

class Scene
{
public:
    Scene() = default;
    ~Scene() { shutdown(); };

    bool isInitialised = false;

    void init(std::shared_ptr<AssetManager> assetManager);
    void update(std::shared_ptr<AssetManager> assetManager, double deltaTime);
    void shutdown();

    Node& getRoot() { return root; }
    const Node& getRoot() const { return root; }
    virtual Camera* getActiveCamera() { return nullptr; }

protected:
    virtual void onInit(std::shared_ptr<AssetManager> assetManager) {} // User hook for scene-specific initialization logic
    virtual void onUpdate(std::shared_ptr<AssetManager> assetManager, double deltaTime) {} // User hook for scene-specific update logic
    virtual void onShutdown() {} // User hook for scene-specific shutdown logic

    void updateNodes(std::shared_ptr<AssetManager> assetManager, double deltaTime) { root.update(assetManager, deltaTime, glm::mat4(1.0f)); }

private:
    Node root;
};
