#pragma once

#include "core/window_config.hpp"
#include "graphics/graphics.hpp"

enum class PlatformType
{
    GLFW,
    // Future platform types can be added here (e.g., SDL, etc.)
};

class IPlatform
{
public:
    IPlatform(WindowConfig* config) : windowConfig(config) {}

    virtual void initialiseWindow() = 0;

    virtual void loadGraphics(IGraphics* graphics) = 0;

    virtual void pollEvents() = 0;

    virtual void swapBuffers() = 0;

    virtual bool shouldClose() = 0;

    virtual double getTime() = 0;

    virtual void terminate() = 0;

protected:
    WindowConfig* windowConfig;
};