#pragma once

#include "core/console/console.hpp"
#include "graphics.hpp"
#include "graphics_opengl.hpp"

class Renderer
{
public:
    Renderer(GraphicsAPI gfxApi) : gfxApi(gfxApi) {
        switch (gfxApi) {
            case GraphicsAPI::OpenGL:
                gfxBackend = new OpenGLGraphics();
                break;
            default:
                Console::get().error("[Renderer] Unknown graphics API!");
                break;
        }
    }

    void clearBackground(const glm::vec4& colour)
    {
        gfxBackend->clearColour(colour);
    }

    
private:
    IGraphics* gfxBackend = nullptr;
    GraphicsAPI gfxApi;
};