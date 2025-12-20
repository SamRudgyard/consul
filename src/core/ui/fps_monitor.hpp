#pragma once

#include <vector>

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
    static constexpr unsigned int MAX_SECONDS_RECORDED = 10;
    std::vector<float> fpsHistory;
    std::vector<float> timeHistory;
    unsigned int writeIndex = 0;
    float currentDeltaTime = 0.0f;
    float currentFps = 0.0f;
    float finalRecordedTime = 0.0f;
};
