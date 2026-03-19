#pragma once

#include "core/engine_context.hpp"
#include "graphics/renderer.hpp"

enum class PlatformType
{
    GLFW,
    // Future platform types can be added here (e.g., SDL, etc.)
};

class Platform
{
public:
    Platform() = default;

    virtual void initialiseWindow() = 0;

    virtual void initialiseImGui(GraphicsAPI gfxApi) = 0;

    virtual void initialiseGraphics(Renderer& renderer) = 0;

    virtual void pollEvents() = 0;

    virtual void swapBuffers() = 0;

    virtual bool shouldClose() = 0;

    virtual void setMousePosition(unsigned int x, unsigned int y) = 0;

    virtual double getTime() = 0;

    virtual void terminate() = 0;

protected:
    EngineContext* context = EngineContext::get();
};
