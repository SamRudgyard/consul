#pragma once

#include "core/console/console.hpp"
#include "core/engine_context.hpp"
#include "platforms/platform.hpp"
#include "graphics/renderer.hpp"
#include "time.hpp"

enum class PlatformType;

class Consul
{
public:

    /**
     * Initialise a Consul application, using default window configuration.
     */
    Consul() { initialiseEngine(); };

    /**
     * Initialise a Consul application with the given window configuration.
     * @param config Window configuration settings.
     */
    Consul(WindowConfig& config) { context->windowConfig = config; initialiseEngine(); };

    ~Consul();

    void initialiseEngine();

    /**
     * Begins a new frame of the Consul application.
     * @returns false when Consul requests termination; true otherwise.
     */
    bool run();

    /**
     * Gracefully terminate the Consul application.
     */
    void terminate();

private:
    Console& console = Console::get();
    EngineContext* context = EngineContext::get();
    IPlatform* platform = nullptr;
    Renderer* renderer = nullptr;

    void initialiseWindow(PlatformType platformType);
    void initialiseRenderer(GraphicsAPI gfxApi);

    void beginTick();
    void endTick();
};