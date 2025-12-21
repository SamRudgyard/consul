#include "fps_monitor.hpp"

#include <algorithm>
#include <numeric>
#include <cstdio>
#include <limits>
#include <vector>

#include "imgui.h"
#include "implot.h"
#include "maths/constants.hpp"
#include "maths/unit_conversions.hpp"

void FpsMonitor::update(double deltaTimeSeconds)
{
    currentDeltaTime = deltaTimeSeconds;
    currentFps = deltaTimeSeconds > 0.0 ? 1.0 / deltaTimeSeconds : 0.0;
    fpsHistory.push_back(currentFps);
    timeHistory.push_back((float)deltaTimeSeconds);

    totalRecordedTime = std::accumulate(timeHistory.begin(), timeHistory.end(), 0.0f);
    while (totalRecordedTime > MAX_SECONDS_RECORDED) {
        fpsHistory.erase(fpsHistory.begin());
        timeHistory.erase(timeHistory.begin());
        totalRecordedTime = std::accumulate(timeHistory.begin(), timeHistory.end(), 0.0f);
    }
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
        ImGui::Text("Frame: %.2f ms (%.0f FPS)", currentDeltaTime*SECONDS_TO_MILLISECONDS, currentFps);

        if (!fpsHistory.empty()) {
            float maxFps = *std::max_element(fpsHistory.begin(), fpsHistory.end());
            float yMax = 1.1f * maxFps;

            std::vector<float> secsAgo;
            secsAgo.reserve(timeHistory.size());
            float elapsedSeconds = totalRecordedTime;
            for (float deltaSeconds : timeHistory) {
                secsAgo.push_back(elapsedSeconds);
                elapsedSeconds -= deltaSeconds;
            }

            if (ImPlot::BeginPlot("##fps_plot", ImVec2(-1, 120.0f), ImPlotFlags_NoLegend)) {
                ImPlot::SetupAxes("Seconds Ago", "FPS", ImPlotAxisFlags_NoGridLines | ImPlotAxisFlags_Invert, ImPlotAxisFlags_NoGridLines);
                ImPlot::SetupAxisLimits(ImAxis_X1, 0.0, MAX_SECONDS_RECORDED, ImPlotCond_Always);
                ImPlot::SetupAxisLimits(ImAxis_Y1, 0.0, yMax, ImPlotCond_Always);
                ImPlot::PlotLine("FPS", secsAgo.data(), fpsHistory.data(), fpsHistory.size());
                ImPlot::EndPlot();
            }
        } else {
            ImGui::TextUnformatted("Collecting data...");
        }
    }
    ImGui::End();
}
