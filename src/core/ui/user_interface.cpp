#include "user_interface.hpp"

UserInterface::UserInterface()
#ifndef CONSUL_CONSOLE_STDOUT
    : consoleWindow(std::make_unique<ConsoleWindow>()),
      windows{consoleWindow.get(), &performanceWindow}
#else
    : windows{&performanceWindow}
#endif
{
}

void UserInterface::update()
{
    for (UIWindow* window : windows) {
        if (window == nullptr) {
            continue;
        }

        window->update();
    }
}
