#pragma once

#ifndef NOMINMAX
#define NOMINMAX // To prevent Windows.h (a dependency of glad/glad.h) defining min/max macros that conflict with std::min/std::max
#endif

#include "graphics.hpp"
#include "glad/glad.h"
#include "utils.hpp"

class OpenGLGraphics : public IGraphics
{
public:
    void loadGraphics(void* loaderFunc) override
    {
        gladLoadGLLoader((GLADloadproc) loaderFunc);

        const GLubyte* version = glGetString(GL_VERSION);
        if (!version) {
            Console::get().error("[OpenGL] Failed to retrieve OpenGL version");
        } else {
            Console::get().log("[OpenGL] Found OpenGL version " + std::string(reinterpret_cast<const char*>(version)));
        }

        glCheckError();
    }

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