#include "user_interface.hpp"

UserInterface::UserInterface()
    : windows{&performanceWindow}
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
