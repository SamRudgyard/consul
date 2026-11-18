#pragma once

#include "core/ecs/ecs.hpp"

class Engine;

class Scene
{
public:
    Scene() = default;
    virtual ~Scene() = default;

    bool isInitialised = false;

    void init();
    void update();
    void shutdown();

    ECS& getECS() { return ecs; }
    const ECS& getECS() const { return ecs; }

protected:
    virtual void onInit() {} // User hook for scene-specific initialization logic
    virtual void onUpdate() {} // User hook for scene-specific update logic
    virtual void onShutdown() {} // User hook for scene-specific shutdown logic

private:
    ECS ecs;
};
