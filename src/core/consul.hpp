#pragma once

#include "core/console/console.hpp"
#include "platforms/platform.hpp"
#include "graphics/renderer.hpp"
#include "input/input_system.hpp"
#include "window_config.hpp"
#include "time.hpp"

enum class PlatformType;

class Consul
{
public:
    WindowConfig windowConfig;

    /**
     * Initialise a Consul application, using default window configuration.
     */
    Consul() { initialiseEngine(); };

    /**
     * Initialise a Consul application with the given window configuration.
     * @param config Window configuration settings.
     */
    Consul(WindowConfig& config) : windowConfig(config) { initialiseEngine(); };

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
    InputSystem& input = InputSystem::get();
    IPlatform* platform = nullptr;
    Renderer* renderer = nullptr;

    void initialiseWindow(PlatformType platformType);
    void initialiseRenderer(GraphicsAPI gfxApi);

    void beginTick();
    void endTick();
};