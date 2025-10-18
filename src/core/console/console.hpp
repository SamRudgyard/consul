#pragma once

#include <string>
#include <vector>

class Console
{
public:
    Console();
    ~Console();

    void ClearLog() { items.clear(); };

    void Log(const char* message);
    void Error(const char* message);
    void Warn(const char* message);
    void Info(const char* message);

    void Draw(const char* title, bool* open = nullptr);

private:
    bool redirectOutput;
    bool autoScroll = true;
    bool scrollToBottom = false;
    std::vector<std::string> items;

};