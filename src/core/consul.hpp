#pragma once

#include "core/console/console.hpp"
#include "platforms/platform.hpp"
#include "graphics/renderer.hpp"
#include "window_config.hpp"
#include "time.hpp"

enum class PlatformType;

class Consul
{
public:
    WindowConfig windowConfig;

    /**
     * Initialise a Consul application.
     */
    Consul();

    /**
     * Initialise a Consul application with the given window configuration.
     * @param config Window configuration settings.
     */
    Consul(WindowConfig& config) : windowConfig(config) { Consul(); };

    ~Consul();

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
    IPlatform* platform = nullptr;
    Renderer* renderer = nullptr;

    void initialiseWindow(PlatformType platformType);
    void initialiseGraphics(GraphicsAPI gfxApi);
};