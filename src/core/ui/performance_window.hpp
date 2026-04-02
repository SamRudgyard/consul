#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "core/engine_context.hpp"
#include "ui_window.hpp"

enum class TimeSpan
{
    LAST_5_SECS = 5,
    LAST_10_SECS = 10,
    LAST_30_SECS = 30,
    LAST_60_SECS = 60
};

class PerformanceWindow : public UIWindow
{
public:
    void update() override;

private:
    bool isOpen = true;
    Time* time = &EngineContext::get()->time;

    // FPS graph
    static constexpr unsigned int MAX_SECONDS_RECORDED = 60;
    std::vector<float> fpsHistory;
    std::vector<float> timeHistory;
    float totalRecordedTime = 0.0f;
    TimeSpan selectedFpsRange = TimeSpan::LAST_30_SECS;
};
