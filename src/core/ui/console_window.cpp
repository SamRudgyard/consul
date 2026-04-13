#include "console_window.hpp"

#include "imgui.h"
#include "core/engine_context.hpp"
#include "core/window.hpp"
#include "utils.hpp"

void ConsoleWindow::update()
{
    if (!isOpen) {
        return;
    }

    EngineContext* context = EngineContext::get();
    Window& window = context->window;

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    const ImVec2 workPos = viewport ? viewport->WorkPos : ImVec2(0.0f, 0.0f);
    const ImVec2 workSize = viewport ? viewport->WorkSize : ImVec2(window.windowSize.x, window.windowSize.y);
    const float consoleWidth = 0.2f * workSize.x;
    const float consoleHeight = 0.3f * workSize.y;

    ImGui::SetNextWindowSize({consoleWidth, consoleHeight}, ImGuiCond_Appearing);
    const ImVec2 padding = ImVec2(1.0f, 1.0f);
    const ImVec2 windowPos = ImVec2(workPos.x + workSize.x - padding.x, workPos.y + workSize.y - padding.y);
    ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always, ImVec2(1.0f, 1.0f));
    if (!ImGui::Begin("Console", &isOpen)) {
        ImGui::End();
        return;
    }

    if (ImGui::BeginPopupContextItem()) {
        if (ImGui::MenuItem("Close")) {
            isOpen = false;
        }
        ImGui::EndPopup();
    }

    if (ImGui::BeginPopup("Options")) {
        ImGui::Checkbox("Auto-scroll", &autoScroll);
        ImGui::EndPopup();
    }
    ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_O, ImGuiInputFlags_Tooltip);
    if (ImGui::Button("Options")) {
        ImGui::OpenPopup("Options");
    }

    if (ImGui::BeginPopupContextWindow()) {
        if (ImGui::Selectable("Clear")) {
            console.clearLog();
        }
        ImGui::EndPopup();
    }

    const float footerHeightToReserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
    if (ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footerHeightToReserve), ImGuiChildFlags_NavFlattened, ImGuiWindowFlags_HorizontalScrollbar)) {
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1));

        for (const std::string& item : console.getItems()) {
            ImVec4 colour;
            bool hasColour = false;
            if (isSubstring(item, "[ERROR]")) {
                colour = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
                hasColour = true;
            } else if (isSubstring(item, "[WARNING]")) {
                colour = ImVec4(1.0f, 1.0f, 0.4f, 1.0f);
                hasColour = true;
            } else if (isSubstring(item, "[INFO]")) {
                colour = ImVec4(0.4f, 0.4f, 1.0f, 1.0f);
                hasColour = true;
            }

            if (hasColour) {
                ImGui::PushStyleColor(ImGuiCol_Text, colour);
            }
            ImGui::TextUnformatted(item.c_str());
            if (hasColour) {
                ImGui::PopStyleColor();
            }
        }

        if (scrollToBottom || (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())) {
            ImGui::SetScrollHereY(1.0f);
        }
        scrollToBottom = false;

        ImGui::PopStyleVar();
    }

    ImGui::EndChild();
    ImGui::Separator();
    ImGui::End();
}
