#pragma once

#include <array>

/**
 * Tracks frame timing and renders an ImGui FPS overlay.
 */
class FpsMonitor
{
public:
    /**
     * Push the latest frame time in seconds.
     */
    void update(double deltaTimeSeconds);

    /**
     * Render the FPS overlay window.
     */
    void draw();

private:
    static constexpr unsigned int HISTORY_SIZE = 256;
    std::array<float, HISTORY_SIZE> fpsHistory{0.0f};
    unsigned int writeIndex = 0;
    float currentDeltaTime = 0.0f;
    float currentFps = 0.0f;
};
