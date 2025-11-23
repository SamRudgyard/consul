#pragma once

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "core/console/console.hpp"
#include "window.hpp"
#include "mouse.hpp"
#include "time.hpp"
#include "keyboard.hpp"

class Consul
{
public:
    /**
     * Initialise a Consul application, with the desired window size.
     *
     * @param title        Window title.
     * @param width        Desired window width in pixels (0 to maximise when windowed).
     * @param height       Desired window height in pixels (0 to maximise when windowed).
     * @param isFullscreen Create the window in fullscreen mode when true.
     */
    Consul(const char* title, unsigned int width, unsigned int height, bool isFullscreen = false);

    /**
     * Initialise a Consul application, either to maximised or fullscreen.
     * @param title        Window title.
     * @param isFullscreen Create the window in fullscreen mode, else maximised.
     */
    Consul(const char* title, bool isFullscreen = false);

    ~Consul();

    /**
     * Begins a new frame of the Consul application.
     * @returns false when Consul requests termination; true otherwise.
     */
    bool Run();

    /**
     * Gracefully terminate the Consul application.
     */
    void Terminate();

    /**
     * Enable or disable vertical sync (vsync), the synchronisation of the frame rate with the monitor's refresh rate.
     * @param enabled True to enable vsync, false to disable.
     */
    void VSync(bool enabled);

private:
    Console& console = Console::Get();
};