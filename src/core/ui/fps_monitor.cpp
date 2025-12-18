#include "fps_monitor.hpp"

#include <algorithm>
#include <cstdio>
#include <limits>

#include "imgui.h"
#include "implot.h"
#include "maths/constants.hpp"
#include "maths/unit_conversions.hpp"

void FpsMonitor::update(float deltaTimeSeconds)
{
    if (deltaTimeSeconds <= 0.0f)
    {
        lastMs = 0.0f;
        lastFps = 0.0f;
    }
    else
    {
        lastMs = deltaTimeSeconds * SECONDS_TO_MILLISECONDS;
        lastFps = 1.0f / deltaTimeSeconds;
    }

    fpsHistory[writeIndex] = lastFps;
    writeIndex = (writeIndex + 1) % HISTORY_SIZE;
    if (writeIndex == 0)
    {
        filled = true;
    }
}

void FpsMonitor::draw()
{
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImVec2 workPos = viewport ? viewport->WorkPos : ImVec2(0.0f, 0.0f);
    ImVec2 workSize = viewport ? viewport->WorkSize : ImVec2(0.0f, 0.0f);

    const ImVec2 padding = ImVec2(10.0f, 10.0f);
    const ImVec2 windowPos = ImVec2(workPos.x + workSize.x - padding.x, workPos.y + padding.y);
    ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always, ImVec2(1.0f, 0.0f));
    ImGui::SetNextWindowBgAlpha(0.8f);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar
        | ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoScrollbar
        | ImGuiWindowFlags_NoSavedSettings
        | ImGuiWindowFlags_NoFocusOnAppearing
        | ImGuiWindowFlags_NoNav;

    if (ImGui::Begin("FPS Monitor", nullptr, flags))
    {
        ImGui::Text("Frame: %.2f ms (%.0f FPS)", lastMs, lastFps);

        const int sampleCount = filled ? static_cast<int>(HISTORY_SIZE) : static_cast<int>(writeIndex);
        if (sampleCount > 0) {
            const int offset = filled ? static_cast<int>(writeIndex) : 0;

            float minFps = MASSIVE;
            float maxFps = TINY;
            for (int i = 0; i < sampleCount; ++i) {
                int idx = (offset + i) % static_cast<int>(HISTORY_SIZE);
                float value = fpsHistory[static_cast<std::size_t>(idx)];
                minFps = std::min(minFps, value);
                maxFps = std::max(maxFps, value);
            }
            if (minFps == MASSIVE) {
                minFps = 0.0f;
            }

            float yMin = 0.0f;
            float yMax = std::max(60.0f, maxFps * 1.2f);

            if (ImPlot::BeginPlot("##fps_plot", ImVec2(-1, 120.0f), ImPlotFlags_NoLegend)) {
                ImPlot::SetupAxes("Frame", "FPS", ImPlotAxisFlags_NoGridLines, ImPlotAxisFlags_NoGridLines);
                ImPlot::SetupAxisLimits(ImAxis_X1, 0, sampleCount > 1 ? sampleCount - 1 : 1, ImPlotCond_Always);
                ImPlot::SetupAxisLimits(ImAxis_Y1, yMin, yMax, ImPlotCond_Always);

                auto getter = [](int idx, void* data) -> ImPlotPoint
                {
                    auto* monitor = static_cast<FpsMonitor*>(data);
                    int offset = monitor->filled ? static_cast<int>(monitor->writeIndex) : 0;
                    int index = (offset + idx) % static_cast<int>(HISTORY_SIZE);
                    return ImPlotPoint(idx, monitor->fpsHistory[static_cast<std::size_t>(index)]);
                };

                ImPlot::PlotLineG("FPS", getter, this, sampleCount);
                ImPlot::EndPlot();
            }
            ImGui::Text("Min %.0f  Max %.0f", minFps, maxFps);
        }
        else
        {
            ImGui::TextUnformatted("Collecting data...");
        }
    }
    ImGui::End();
}
