#include "user_interface.hpp"

UserInterface::UserInterface()
    : windows{&consoleWindow, &performanceWindow}
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
