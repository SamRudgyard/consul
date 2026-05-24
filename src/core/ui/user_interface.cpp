#include "user_interface.hpp"
#include "core/profiling/profile_method.hpp"
#include "graphics/material/material.hpp"

UserInterface::UserInterface()
#ifndef CONSUL_CONSOLE_STDOUT
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
    CONSUL_PROFILE_METHOD();

    for (UIWindow* window : windows) {
        if (window == nullptr) {
            continue;
        }

        window->update();
    }
}
