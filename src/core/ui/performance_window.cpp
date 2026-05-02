#include "performance_window.hpp"

#include <algorithm>
#include <array>
#include <limits>
#include <utility>

#include "imgui.h"
#include "implot.h"
#include "maths/unit_conversions.hpp"
#include "utils.hpp"

namespace
{
constexpr std::array<TimeSpan, 4> SELECTABLE_TIME_RANGES = {
    TimeSpan::LAST_5_SECS,
    TimeSpan::LAST_10_SECS,
    TimeSpan::LAST_30_SECS,
    TimeSpan::LAST_60_SECS
};

constexpr std::array<const char*, 4> SELECTABLE_TIME_RANGE_LABELS = {
    "Last 5s",
    "Last 10s",
    "Last 30s",
    "Last 60s"
};

void drawTimeRangeCombo(const char* label, TimeSpan& selectedTimeRange)
{
    const auto selectedIt = std::find(SELECTABLE_TIME_RANGES.begin(), SELECTABLE_TIME_RANGES.end(), selectedTimeRange);
    int selectedIndex = selectedIt == SELECTABLE_TIME_RANGES.end()
        ? 0
        : (int)(std::distance(SELECTABLE_TIME_RANGES.begin(), selectedIt));

    ImGui::SetNextItemWidth(100.0f);
    if (ImGui::Combo(label, &selectedIndex, SELECTABLE_TIME_RANGE_LABELS.data(), (int)(SELECTABLE_TIME_RANGE_LABELS.size()))) {
        selectedTimeRange = SELECTABLE_TIME_RANGES.at((std::size_t)(selectedIndex));
    }
}

std::string getProfilerLegendMethodLabel(const std::string& fullMethodName)
{
    // Drop argument/signature details.
    std::string label = fullMethodName;
    const std::size_t signatureStart = label.find('(');
    if (signatureStart != std::string::npos) {
        label = label.substr(0, signatureStart);
    }

    // Drop return type/calling convention prefixes.
    const std::size_t lastSpace = label.find_last_of(' ');
    if (lastSpace != std::string::npos) {
        label = label.substr(lastSpace + 1);
    }

    return label;
}
} // namespace

void PerformanceWindow::update()
{
    if (time == nullptr) {
        return;
    }

    const float deltaTime = (float)(time->deltaTime);
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
    const ImVec2 padding = ImVec2(1.0f, 1.0f);
    const ImVec2 windowPos = ImVec2(workPos.x + workSize.x - padding.x, workPos.y + padding.y);
    ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always, ImVec2(1.0f, 0.0f));

    if (!ImGui::Begin("Performance", &isOpen)) {
        ImGui::End();
        return;
    }

    if (ImGui::BeginTabBar("##performance_tabs")) {
        if (ImGui::BeginTabItem("Overview")) {
            if (ImGui::CollapsingHeader("Time", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Text("Run Time:    %.3f s", time->currentTime);
                ImGui::Text("Update Time: %.3f ms", time->updateTime*SECONDS_TO_MILLISECONDS);
                ImGui::Text("Render Time: %.3f ms", time->renderTime*SECONDS_TO_MILLISECONDS);
                ImGui::Text("Frame Time:  %.3f ms", time->deltaTime*SECONDS_TO_MILLISECONDS);
                ImGui::Text("FPS:         %.0f", time->deltaTime > 0.0 ? 1.0 / time->deltaTime : 0.0);
            }

            if (ImGui::CollapsingHeader("FPS Plot", ImGuiTreeNodeFlags_DefaultOpen)) {
                drawTimeRangeCombo("Time Range##fps_range", selectedFpsRange);

                if (fpsHistory.empty()) {
                    ImGui::TextUnformatted("Collecting data...");
                } else {
                    float maxFps = *std::max_element(fpsHistory.begin(), fpsHistory.end());
                    float yMax = std::max(1.0f, 1.1f*maxFps);

                    std::vector<float> secsAgo;
                    secsAgo.reserve(timeHistory.size());
                    float elapsedSeconds = totalRecordedTime;
                    for (float deltaSeconds : timeHistory) {
                        secsAgo.push_back(elapsedSeconds);
                        elapsedSeconds -= deltaSeconds;
                    }

                    if (ImPlot::BeginPlot("##fps_plot", ImVec2(-1, 120.0f), ImPlotFlags_NoLegend)) {
                        ImPlot::SetupAxes("Seconds Ago", "FPS", ImPlotAxisFlags_NoGridLines | ImPlotAxisFlags_Invert, ImPlotAxisFlags_NoGridLines);
                        ImPlot::SetupAxisLimits(ImAxis_X1, 0.0, (double)(selectedFpsRange), ImPlotCond_Always);
                        ImPlot::SetupAxisLimits(ImAxis_Y1, 0.0, yMax, ImPlotCond_Always);
                        ImPlot::PlotLine("FPS", secsAgo.data(), fpsHistory.data(), (int)(fpsHistory.size()));
                        ImPlot::EndPlot();
                    }
                }
            }

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Profiler")) {
            if (profiler == nullptr) {
                ImGui::TextUnformatted("Profiler unavailable.");
                ImGui::EndTabItem();
                ImGui::EndTabBar();
                ImGui::End();
                return;
            }

            const std::vector<std::string>& methods = profiler->getRegisteredMethods();
            if (methods.empty()) {
                ImGui::TextUnformatted("No methods profiled yet.");
                ImGui::TextUnformatted("Use CONSUL_PROFILE_METHOD() or CONSUL_PROFILE_SCOPE(\"name\") in methods you want to track.");
                ImGui::EndTabItem();
                ImGui::EndTabBar();
                ImGui::End();
                return;
            }

            if (selectedProfilerMethods.size() < methods.size()) {
                selectedProfilerMethods.resize(methods.size(), true);
            } else if (selectedProfilerMethods.size() > methods.size()) {
                selectedProfilerMethods.resize(methods.size());
            }

            std::vector<std::string> methodLabels;
            methodLabels.reserve(methods.size());
            for (const std::string& methodName : methods) {
                methodLabels.push_back(getProfilerLegendMethodLabel(methodName));
            }
            const std::vector<std::size_t> alphabetizedMethodIndices = getAlphabeticalStringOrder(methodLabels);

            const auto countSelectedMethods = [this]() -> int {
                int selectedCount = 0;
                for (bool isSelected : selectedProfilerMethods) {
                    if (isSelected) {
                        selectedCount++;
                    }
                }
                return selectedCount;
            };
            int nSelectedMethods = countSelectedMethods();

            std::string methodsPreview = "Methods";
            if (nSelectedMethods == (int)(methods.size())) {
                methodsPreview = "All Methods";
            } else if (nSelectedMethods == 0) {
                methodsPreview = "No Methods";
            } else {
                methodsPreview = std::to_string(nSelectedMethods) + " / " + std::to_string(methods.size()) + " Methods";
            }

            ImGui::SetNextItemWidth(300.0f);
            if (ImGui::BeginCombo("Methods", methodsPreview.c_str())) {
                if (ImGui::Selectable("Select All")) {
                    std::fill(selectedProfilerMethods.begin(), selectedProfilerMethods.end(), true);
                }
                if (ImGui::Selectable("Clear All")) {
                    std::fill(selectedProfilerMethods.begin(), selectedProfilerMethods.end(), false);
                }
                ImGui::Separator();

                for (std::size_t sortedIndex = 0; sortedIndex < alphabetizedMethodIndices.size(); sortedIndex++) {
                    const std::size_t methodIndex = alphabetizedMethodIndices[sortedIndex];
                    const std::string& methodLabel = methodLabels[methodIndex];
                    bool isEnabled = selectedProfilerMethods[methodIndex];
                    ImGui::PushID((int)(methodIndex));
                    if (ImGui::Checkbox(methodLabel.c_str(), &isEnabled)) {
                        selectedProfilerMethods[methodIndex] = isEnabled;
                    }
                    ImGui::PopID();
                }
                ImGui::EndCombo();
            }
            nSelectedMethods = countSelectedMethods();

            drawTimeRangeCombo("Time Range##profiler_range", selectedProfilerRange);

            const std::vector<float>& frameDurations = profiler->getFrameDurations();

            if (nSelectedMethods == 0) {
                ImGui::TextUnformatted("No methods selected.");
                ImGui::EndTabItem();
                ImGui::EndTabBar();
                ImGui::End();
                return;
            }

            if (frameDurations.empty()) {
                ImGui::TextUnformatted("Collecting profiler data...");
                ImGui::EndTabItem();
                ImGui::EndTabBar();
                ImGui::End();
                return;
            }

            std::size_t sampleCount = frameDurations.size();
            for (std::size_t i = 0; i < methods.size(); i++) {
                if (!selectedProfilerMethods[i]) {
                    continue;
                }

                const std::vector<float>* methodHistoryMs = profiler->getMethodHistoryMs(methods[i]);
                if (methodHistoryMs == nullptr || methodHistoryMs->empty()) {
                    continue;
                }

                sampleCount = std::min(sampleCount, methodHistoryMs->size());
            }

            if (sampleCount == 0) {
                ImGui::TextUnformatted("Collecting profiler data...");
                ImGui::EndTabItem();
                ImGui::EndTabBar();
                ImGui::End();
                return;
            }

            const std::size_t frameStartIndex = frameDurations.size() - sampleCount;
            std::vector<float> secsAgo;
            secsAgo.reserve(sampleCount);

            float elapsedSeconds = 0.0f;
            for (std::size_t i = frameStartIndex; i < frameDurations.size(); i++) {
                elapsedSeconds += frameDurations[i];
            }

            for (std::size_t i = frameStartIndex; i < frameDurations.size(); i++) {
                secsAgo.push_back(elapsedSeconds);
                elapsedSeconds -= frameDurations[i];
            }

            const float selectedRangeSeconds = (float)(selectedProfilerRange);

            float visibleMinMs = std::numeric_limits<float>::max();
            float visibleMaxMs = 0.0f;
            std::vector<float> summedSelectedMethodMs(sampleCount, 0.0f);
            int visibleSampleCount = 0;
            std::vector<std::pair<std::size_t, float>> methodRuntimeOrder;

            for (std::size_t methodIndex = 0; methodIndex < methods.size(); methodIndex++) {
                if (!selectedProfilerMethods[methodIndex]) {
                    continue;
                }

                const std::vector<float>* methodHistoryMs = profiler->getMethodHistoryMs(methods[methodIndex]);
                if (methodHistoryMs == nullptr || methodHistoryMs->size() < sampleCount) {
                    continue;
                }

                const std::size_t methodStartIndex = methodHistoryMs->size() - sampleCount;
                const float* methodData = methodHistoryMs->data() + methodStartIndex;
                float methodVisibleTotalMs = 0.0f;
                int methodVisibleCount = 0;

                for (std::size_t i = 0; i < sampleCount; i++) {
                    if (secsAgo[i] > selectedRangeSeconds) {
                        continue;
                    }
                    visibleMinMs = std::min(visibleMinMs, methodData[i]);
                    visibleMaxMs = std::max(visibleMaxMs, methodData[i]);
                    summedSelectedMethodMs[i] += methodData[i];
                    methodVisibleTotalMs += methodData[i];
                    methodVisibleCount++;
                    visibleSampleCount++;
                }

                const float methodAverageRuntimeMs = methodVisibleCount > 0
                    ? methodVisibleTotalMs/(float)(methodVisibleCount)
                    : 0.0f;
                methodRuntimeOrder.emplace_back(methodIndex, methodAverageRuntimeMs);
            }

            std::sort(methodRuntimeOrder.begin(), methodRuntimeOrder.end(), [](const auto& lhs, const auto& rhs) {
                return lhs.second > rhs.second;
            });

            float yMin = 0.0f;
            float yMax = 0.1f;
            if (visibleSampleCount > 0) {
                const float ySpan = std::max(0.1f, visibleMaxMs - visibleMinMs);
                const float yPadding = ySpan*0.1f;
                yMin = std::max(0.0f, visibleMinMs - yPadding);
                yMax = std::max(yMin + 0.1f, visibleMaxMs + yPadding);
            }

            float latestMs = summedSelectedMethodMs[sampleCount - 1];

            ImGui::Text("Selected Methods: %d", nSelectedMethods);
            ImGui::SameLine();
            ImGui::Text("Latest Sum: %.3f ms", latestMs);

            constexpr float profilerPlotHeight = 300.0f;
            constexpr float profilerLegendWidth = 100.0f;
            constexpr float profilerLegendSpacing = 8.0f;
            const float availableWidth = ImGui::GetContentRegionAvail().x;
            const float plotWidth = std::max(120.0f, availableWidth - profilerLegendWidth - profilerLegendSpacing);

            if (ImPlot::BeginPlot("##profiler_plot", ImVec2(plotWidth, profilerPlotHeight), ImPlotFlags_NoLegend)) {
                ImPlot::SetupAxes("Seconds Ago", "Runtime (ms)", ImPlotAxisFlags_NoGridLines | ImPlotAxisFlags_Invert, ImPlotAxisFlags_NoGridLines);
                ImPlot::SetupAxisLimits(ImAxis_X1, 0.0, selectedRangeSeconds, ImPlotCond_Always);
                ImPlot::SetupAxisLimits(ImAxis_Y1, yMin, yMax, ImPlotCond_Always);
                for (std::size_t plotOrderIndex = 0; plotOrderIndex < methodRuntimeOrder.size(); plotOrderIndex++) {
                    const std::size_t methodIndex = methodRuntimeOrder[plotOrderIndex].first;
                    const std::vector<float>* methodHistoryMs = profiler->getMethodHistoryMs(methods[methodIndex]);
                    if (methodHistoryMs == nullptr || methodHistoryMs->size() < sampleCount) {
                        continue;
                    }

                    const std::size_t methodStartIndex = methodHistoryMs->size() - sampleCount;
                    const float* methodData = methodHistoryMs->data() + methodStartIndex;
                    ImPlot::SetNextLineStyle(ImPlot::GetColormapColor((int)(plotOrderIndex)));
                    ImPlot::PlotLine(methods[methodIndex].c_str(), secsAgo.data(), methodData, (int)(sampleCount));
                }
                ImPlot::EndPlot();
            }

            ImGui::SameLine(0.0f, profilerLegendSpacing);
            if (ImGui::BeginChild("##profiler_legend_side_panel", ImVec2(profilerLegendWidth, profilerPlotHeight), true, ImGuiWindowFlags_HorizontalScrollbar)) {
                ImGui::TextUnformatted("Legend (Desc)");
                ImGui::Separator();

                for (std::size_t legendOrderIndex = 0; legendOrderIndex < methodRuntimeOrder.size(); legendOrderIndex++) {
                    const std::size_t methodIndex = methodRuntimeOrder[legendOrderIndex].first;
                    const std::string methodLabel = getProfilerLegendMethodLabel(methods[methodIndex]);

                    const std::string colorButtonId = "##profiler_legend_color_" + std::to_string(legendOrderIndex);
                    ImGui::ColorButton(
                        colorButtonId.c_str(),
                        ImPlot::GetColormapColor((int)(legendOrderIndex)),
                        ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoDragDrop | ImGuiColorEditFlags_NoBorder,
                        ImVec2(10.0f, 10.0f)
                    );
                    ImGui::SameLine();
                    ImGui::TextUnformatted(methodLabel.c_str());
                }
            }
            ImGui::EndChild();

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::End();
}
