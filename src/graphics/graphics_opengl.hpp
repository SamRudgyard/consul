#pragma once

#include "graphics.hpp"

class OpenGLGraphics : public IGraphics
{
public:
    void clearColour(const glm::vec4& colour) override
    {
        glClearColor(colour.r, colour.g, colour.b, colour.a);
        clearScreenBuffer();
    }

    void clearScreenBuffer()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
};