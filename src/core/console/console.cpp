#include "console.hpp"

#include <iostream>
#include <stdexcept>

Console::~Console()
{
    clearLog();
}

void Console::appendMessage(const std::string& message)
{
#ifdef CONSUL_CONSOLE_STDOUT
    std::cout << message << '\n';
    return;
#else
    if (items.size() >= MAX_CONSOLE_SIZE) {
        items.erase(items.begin());
    }
    items.push_back(message);
#endif
}

void Console::log(const std::string& message)
{
    appendMessage(message);
}

void Console::logOnDebug(const std::string& message)
{
#ifndef NDEBUG
    appendMessage("[DEBUG] " + message);
#endif
}

void Console::error(const std::string& message)
{
    appendMessage("[ERROR] " + message);
    throw std::runtime_error(message);
}

void Console::warn(const std::string& message)
{
    appendMessage("[WARNING] " + message);
}

void Console::info(const std::string& message)
{
    appendMessage("[INFO] " + message);
}
