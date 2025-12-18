#pragma once

#include <algorithm>
#include <functional>
#include <string>
#include <vector>

/**
 * Lightweight ImGui wrapper that manages renderable UI windows.
 * Windows register a draw callback; the UI iterates and renders them each frame.
 */
class UserInterface
{
public:
    using DrawCallback = std::function<void(const std::string& name, bool* open)>;

    struct Window
    {
        std::string name;
        DrawCallback draw;
        bool* open;
    };

    /**
     * Register or update a window with a draw callback.
     * If a window with the same name exists, its callback and open state are updated.
     */
    void registerWindow(const std::string& name, DrawCallback draw, bool* open);

    /**
     * Unregister a window by name.
     */
    void unregisterWindow(const std::string& name);

    /**
     * Render all registered windows.
     */
    void render();

private:
    std::vector<Window> windows;
};
