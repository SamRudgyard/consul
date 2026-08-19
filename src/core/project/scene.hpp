#pragma once

#include "core/ecs/ecs.hpp"

class Camera;
class Engine;

class Scene
{
public:
    Scene() = default;
    virtual ~Scene() = default;

    bool isInitialised = false;

    void init(Engine& engine);
    void update(Engine& engine, double deltaTime);
    void shutdown();

    ECS& getECS() { return ecs; }
    const ECS& getECS() const { return ecs; }
    virtual Camera* getActiveCamera() { return nullptr; }

protected:
    virtual void onInit(Engine& engine) {} // User hook for scene-specific initialization logic
    virtual void onUpdate(Engine& engine, double deltaTime) {} // User hook for scene-specific update logic
    virtual void onShutdown() {} // User hook for scene-specific shutdown logic

private:
    ECS ecs;
};
