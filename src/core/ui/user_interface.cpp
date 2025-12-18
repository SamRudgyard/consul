#include "user_interface.hpp"

#include <utility>

#include "core/console/console.hpp"

void UserInterface::registerWindow(const std::string& name, DrawCallback draw, bool* open)
{
    // Check if window with the same name already exists - if so, overwrite it
    for (auto& window : windows) {
        if (window.name == name) {
            window.draw = std::move(draw);
            window.open = open;
            return;
        }
    }

    windows.push_back(Window{name, std::move(draw), open});
}

void UserInterface::unregisterWindow(const std::string& name)
{
    // Only remove windows that match the given name
    const auto isFoundIter = std::remove_if(windows.begin(), windows.end(), [&](const Window& window) { return window.name == name; });
    windows.erase(isFoundIter, windows.end());
}

void UserInterface::render()
{
    for (auto& window : windows) {
        const std::string& name = window.name;
        bool* open = window.open;

        if (open && !*open) {
            continue;
        }

        if (!window.draw) {
            Console::get().warn("[UserInterface] Window '" + name + "' has no draw callback registered!");
            continue;
        }
        
        window.draw(name, open);
    }
}
