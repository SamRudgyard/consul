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
    void update(float deltaTimeSeconds);

    /**
     * Render the FPS overlay window.
     */
    void draw();

private:
    static constexpr std::size_t HISTORY_SIZE = 256;
    std::array<float, HISTORY_SIZE> fpsHistory{}; // ring buffer of FPS values
    std::size_t writeIndex = 0;
    bool filled = false;
    float lastMs = 0.0f;
    float lastFps = 0.0f;
};
