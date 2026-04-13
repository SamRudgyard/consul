#pragma once

#include "ui_window.hpp"
#include "core/console/console.hpp"

class ConsoleWindow : public UIWindow
{
public:
    void update() override;

private:
    bool isOpen = true;
    bool autoScroll = true;
    bool scrollToBottom = false;
    Console& console = Console::get();
};
