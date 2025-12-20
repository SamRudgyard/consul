#include "fps_monitor.hpp"

#include <algorithm>
#include <cstdio>
#include <limits>

#include "imgui.h"
#include "implot.h"
#include "maths/constants.hpp"
#include "maths/unit_conversions.hpp"

void FpsMonitor::update(double deltaTimeSeconds)
{
    currentDeltaTime = deltaTimeSeconds;
    currentFps = deltaTimeSeconds > 0.0 ? 1.0 / deltaTimeSeconds : 0.0;
    fpsHistory[writeIndex] = currentFps;
    writeIndex = (writeIndex + 1) % HISTORY_SIZE;
}

void FpsMonitor::draw()
{
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImVec2 workPos = viewport ? viewport->WorkPos : ImVec2(0.0f, 0.0f);
    ImVec2 workSize = viewport ? viewport->WorkSize : ImVec2(0.0f, 0.0f);

    // Place the overlay in the top-right corner of the main viewport.
    const ImVec2 padding = ImVec2(10.0f, 10.0f);
    const ImVec2 windowPos = ImVec2(workPos.x + workSize.x - padding.x, workPos.y + padding.y);
    ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always, ImVec2(1.0f, 0.0f));
    // ImGui::SetNextWindowBgAlpha(0.8f);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar
        | ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoScrollbar
        | ImGuiWindowFlags_NoSavedSettings
        | ImGuiWindowFlags_NoFocusOnAppearing
        | ImGuiWindowFlags_NoNav;

    if (ImGui::Begin("FPS Monitor", nullptr, flags))
    {
        ImGui::Text("Frame: %.2f ms (%.0f FPS)", currentDeltaTime, currentFps);

        float yMin = 0.0f;
        float maxFPS = *std::max_element(std::begin(fpsHistory), std::end(fpsHistory));
        float yMax = 1.1f*maxFPS;

        if (ImPlot::BeginPlot("##fps_plot", ImVec2(-1, 120.0f), ImPlotFlags_NoLegend)) {
            ImPlot::SetupAxes("Frame", "FPS", ImPlotAxisFlags_NoGridLines, ImPlotAxisFlags_NoGridLines);
            ImPlot::SetupAxisLimits(ImAxis_X1, 0, HISTORY_SIZE - 1, ImPlotCond_Always);
            ImPlot::SetupAxisLimits(ImAxis_Y1, yMin, yMax, ImPlotCond_Always);

            auto getter = [](int idx, void* data) -> ImPlotPoint
            {
                auto* monitor = (FpsMonitor*)(data);
                unsigned int offset = monitor->writeIndex;
                unsigned int index = (offset + idx) % HISTORY_SIZE;
                return ImPlotPoint(idx, monitor->fpsHistory[index]);
            };

            ImPlot::PlotLineG("FPS", getter, this, HISTORY_SIZE);
            ImPlot::EndPlot();
        }
    }
    ImGui::End();
}
