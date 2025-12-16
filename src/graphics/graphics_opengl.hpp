#pragma once

#ifndef NOMINMAX
#define NOMINMAX // To prevent Windows.h (a dependency of glad/glad.h) defining min/max macros that conflict with std::min/std::max
#endif

#include "graphics.hpp"
#include "glad/glad.h"
#include "shaders/opengl/opengl_shader.hpp"
#include "graphics/mesh/opengl/opengl_mesh.hpp"
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

        glEnable(GL_DEPTH_TEST);                                // Enable depth testing
        glDepthFunc(GL_LESS);                                   // Type of depth testing to apply
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);      // Colour blending, determines how pixel colours are combined
        glEnable(GL_BLEND);                                     // Enable colour blending (required for transparencies)
        glCullFace(GL_BACK);                                    // Cull back faces
        glFrontFace(GL_CCW);                                    // Front faces are counter clockwise
        glEnable(GL_CULL_FACE);                                 // Enable backface culling
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

    IShader* newShader() override
    {
        return new OpenGLShader();
    }

    RenderableMesh* newMesh(Mesh& mesh) override
    {
        return new OpenGLMesh(mesh);
    }
};