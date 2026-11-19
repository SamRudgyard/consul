#include "user_interface.hpp"

#include "core/profiling/profile_method.hpp"
#include "graphics/material/material.hpp"

UserInterface::UserInterface()
#ifndef CREMA_CONSOLE_STDOUT
    : consoleWindow(std::make_unique<ConsoleWindow>()),
      windows{consoleWindow.get(), &performanceWindow}
#else
    : windows{&performanceWindow}
#endif
{
    Material::registerProperties();
}

void UserInterface::update()
{
    CREMA_PROFILE_METHOD();

    for (UIWindow* window : windows) {
        if (window == nullptr) {
            continue;
        }

        window->update();
    }
}
