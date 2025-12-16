#include "console.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "core/engine_context.hpp"
#include "core/window.hpp"
#include "utils.hpp"

Console::~Console()
{
    clearLog();
}

void Console::log(const std::string& message)
{
    if (items.size() > MAX_CONSOLE_SIZE) {
        items.pop_front();
    }
    items.push_back(message);
}

void Console::logOnDebug(const std::string& message)
{
#ifndef NDEBUG
    if (items.size() > MAX_CONSOLE_SIZE) {
        items.pop_front();
    }
    items.push_back("[DEBUG] " + message);
#endif
}

void Console::error(const std::string& message)
{
    if (items.size() > MAX_CONSOLE_SIZE) {
        items.pop_front();
    }
    items.push_back("[ERROR] " + message);

    throw std::runtime_error(message);
}

void Console::warn(const std::string& message)
{
    if (items.size() > MAX_CONSOLE_SIZE) {
        items.pop_front();
    }
    items.push_back("[WARNING] " + message);
}

void Console::info(const std::string& message)
{
    if (items.size() > MAX_CONSOLE_SIZE) {
        items.pop_front();
    }
    items.push_back("[INFO] " + message);
}

void Console::draw(const char* title, bool* open)
{
    EngineContext* context = EngineContext::get();
    Window& window = context->window;
    unsigned int consoleWidth = (int)(0.2f * window.windowSize.x);
    unsigned int consoleHeight = (int)(0.3f * window.windowSize.y);
    ImGui::SetNextWindowSize({(float)consoleWidth, (float)consoleHeight}, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos({(float)(window.windowSize.x - consoleWidth), (float)(window.windowSize.y - consoleHeight)}, ImGuiCond_FirstUseEver);
    if (!ImGui::Begin(title, open))
    {
        ImGui::End();
        return;
    }

    if (ImGui::BeginPopupContextItem())
    {
        if (ImGui::MenuItem("Close"))
        {
            *open = false;
        }
        ImGui::EndPopup();
    }

    if (ImGui::BeginPopup("Options"))
    {
        ImGui::Checkbox("Auto-scroll", &autoScroll);
        ImGui::EndPopup();
    }
    ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_O, ImGuiInputFlags_Tooltip);
    if (ImGui::Button("Options"))
    {
        ImGui::OpenPopup("Options");
    }

    if (ImGui::BeginPopupContextWindow())
    {
        if (ImGui::Selectable("Clear")) clearLog();
        ImGui::EndPopup();
    }

    const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
    if (ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), ImGuiChildFlags_NavFlattened, ImGuiWindowFlags_HorizontalScrollbar))
    {
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1));

        for (std::string item : items)
        {
            ImVec4 colour;
            bool hasColour = false;
            if (isSubstring(item, "[ERROR]"))
            {
                colour = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
                hasColour = true;
            }
            else if (isSubstring(item, "[WARNING]"))
            {
                colour = ImVec4(1.0f, 1.0f, 0.4f, 1.0f);
                hasColour = true;
            }
            else if (isSubstring(item, "[INFO]"))
            {
                colour = ImVec4(0.4f, 0.4f, 1.0f, 1.0f);
                hasColour = true;
            }
            if (hasColour) ImGui::PushStyleColor(ImGuiCol_Text, colour);
            ImGui::TextUnformatted(item.c_str());
            if (hasColour) ImGui::PopStyleColor();
        }

        if (scrollToBottom || (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
        {
            ImGui::SetScrollHereY(1.0f);
        }
        scrollToBottom = false;

        ImGui::PopStyleVar();
    }

    ImGui::EndChild();
    ImGui::Separator();

    ImGui::End();
}
