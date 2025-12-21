#pragma once

#include <vector>
#include <map>

enum class XLimit
{
    LAST_5_SECS = 5,
    LAST_10_SECS = 10,
    LAST_30_SECS = 30,
    LAST_60_SECS = 60
};

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
    void draw(const std::string& title, bool* open);

private:
    static constexpr unsigned int MAX_SECONDS_RECORDED = 60;
    std::vector<float> fpsHistory;
    std::vector<float> timeHistory;
    float currentDeltaTime = 0.0f;
    float currentFps = 0.0f;
    float finalRecordedTime = 0.0f;
    float totalRecordedTime = 0.0f;
    XLimit selectedFpsRange = XLimit::LAST_30_SECS;
    std::map<const char*, XLimit> xRanges = {
            {"Last 5s", XLimit::LAST_5_SECS},
            {"Last 10s", XLimit::LAST_10_SECS},
            {"Last 30s", XLimit::LAST_30_SECS},
            {"Last 60s", XLimit::LAST_60_SECS}
        };
};
