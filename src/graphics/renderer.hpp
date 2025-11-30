#pragma once

#include "core/console/console.hpp"
#include "graphics.hpp"
#include "graphics_opengl.hpp"
#include "platforms/platform.hpp"

class Renderer
{
public:
    Renderer(IPlatform* platform, GraphicsAPI gfxApi) : gfxApi(gfxApi) {
        switch (gfxApi) {
            case GraphicsAPI::OpenGL:
                gfxBackend = new OpenGLGraphics();
                break;
            default:
                Console::get().error("[Renderer] Unknown graphics API!");
                break;
        }

        if (platform && gfxBackend) {
            platform->loadGraphics(gfxBackend);
        }
    }

    ~Renderer() {
        delete gfxBackend;
    }

    void clearBackground(const glm::vec4& colour)
    {
        if (!gfxBackend) {
            Console::get().error("[Renderer] Attempted to call clearBackground but no graphics backend exists!");
            return;
        }

        assert(gfxBackend != nullptr && "Graphics backend not initialized!");

        gfxBackend->clearColour(colour);
    }

    
private:
    IGraphics* gfxBackend = nullptr;
    GraphicsAPI gfxApi;
};