#pragma once

#include "glm/vec4.hpp"

enum class GraphicsAPI
{
    OpenGL,
    // Future graphics APIs can be added here (e.g., Vulkan, DirectX, etc.)
};

class IGraphics
{
public:
    virtual void loadGraphics(void* loaderFunc) = 0;
    virtual void clearColour(const glm::vec4& colour) = 0;
};