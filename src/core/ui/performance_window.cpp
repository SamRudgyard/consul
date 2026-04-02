#include "performance_window.hpp"

#include <algorithm>

#include "imgui.h"
#include "implot.h"
#include "maths/unit_conversions.hpp"

void PerformanceWindow::update()
{
    if (time == nullptr) {
        return;
    }

    const float deltaTime = time->deltaTime;
    const float currentFps = deltaTime > 0.0f ? 1.0f / deltaTime : 0.0f;

    fpsHistory.push_back(currentFps);
    timeHistory.push_back(deltaTime);
    totalRecordedTime += deltaTime;

    while (totalRecordedTime > MAX_SECONDS_RECORDED && !timeHistory.empty()) {
        totalRecordedTime -= timeHistory.front();
        timeHistory.erase(timeHistory.begin());
        fpsHistory.erase(fpsHistory.begin());
    }

    if (!isOpen) {
        return;
    }

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImVec2 workPos = viewport ? viewport->WorkPos : ImVec2(0.0f, 0.0f);
    ImVec2 workSize = viewport ? viewport->WorkSize : ImVec2(0.0f, 0.0f);

    // Place the overlay in the top-right corner of the main viewport.
    const ImVec2 padding = ImVec2(10.0f, 10.0f);
    const ImVec2 windowPos = ImVec2(workPos.x + workSize.x - padding.x, workPos.y + padding.y);
    ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always, ImVec2(1.0f, 0.0f));

    if (!ImGui::Begin("Performance", &isOpen)) {
        ImGui::End();
        return;
    }

    if (ImGui::CollapsingHeader("Time", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Text("Run Time: %.3f s", time->currentTime);
        ImGui::Text("Update Time: %.3f ms", time->updateTime*SECONDS_TO_MILLISECONDS);
        ImGui::Text("Render Time: %.3f ms", time->renderTime*SECONDS_TO_MILLISECONDS);
        ImGui::Text("Frame Time: %.3f ms", time->deltaTime*SECONDS_TO_MILLISECONDS);
        ImGui::Text("FPS: %.0f", time->deltaTime > 0.0 ? 1.0 / time->deltaTime : 0.0);
    }

    if (ImGui::CollapsingHeader("FPS Plot", ImGuiTreeNodeFlags_DefaultOpen)) {
        static const std::vector<TimeSpan> selectableTimeRanges = {
            TimeSpan::LAST_5_SECS,
            TimeSpan::LAST_10_SECS,
            TimeSpan::LAST_30_SECS,
            TimeSpan::LAST_60_SECS
        };

        static const std::vector<const char*> selectableTimeRangeLabels = {
            "Last 5s",
            "Last 10s",
            "Last 30s",
            "Last 60s"
        };

        int selectedIndex = std::distance(selectableTimeRanges.begin(), std::find(selectableTimeRanges.begin(), selectableTimeRanges.end(), selectedFpsRange));
        
        ImGui::SetNextItemWidth(100.0f);
        ImGui::Combo("Select Time Range", &selectedIndex, selectableTimeRangeLabels.data(), static_cast<int>(selectableTimeRangeLabels.size()));
        selectedFpsRange = selectableTimeRanges.at(selectedIndex);

        if (fpsHistory.empty()) {
            ImGui::TextUnformatted("Collecting data...");
            return;
        }

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
            ImPlot::SetupAxisLimits(ImAxis_X1, 0.0, (float)selectedFpsRange, ImPlotCond_Always);
            ImPlot::SetupAxisLimits(ImAxis_Y1, 0.0, yMax, ImPlotCond_Always);
            ImPlot::PlotLine("FPS", secsAgo.data(), fpsHistory.data(), fpsHistory.size());
            ImPlot::EndPlot();
        }
    }

    ImGui::End();
}
