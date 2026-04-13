#include "console.hpp"

#include <stdexcept>

Console::~Console()
{
    clearLog();
}

void Console::log(const std::string& message)
{
    if (items.size() > MAX_CONSOLE_SIZE) {
        items.erase(items.begin());
    }
    items.push_back(message);
}

void Console::logOnDebug(const std::string& message)
{
#ifndef NDEBUG
    if (items.size() > MAX_CONSOLE_SIZE) {
        items.erase(items.begin());
    }
    items.push_back("[DEBUG] " + message);
#endif
}

void Console::error(const std::string& message)
{
    if (items.size() > MAX_CONSOLE_SIZE) {
        items.erase(items.begin());
    }
    items.push_back("[ERROR] " + message);

    throw std::runtime_error(message);
}

void Console::warn(const std::string& message)
{
    if (items.size() > MAX_CONSOLE_SIZE) {
        items.erase(items.begin());
    }
    items.push_back("[WARNING] " + message);
}

void Console::info(const std::string& message)
{
    if (items.size() > MAX_CONSOLE_SIZE) {
        items.erase(items.begin());
    }
    items.push_back("[INFO] " + message);
}
