#pragma once

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
};
