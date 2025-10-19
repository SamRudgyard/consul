#pragma once

#include <string>
#include <vector>

class Console
{
public:
    Console(const Console&) = delete;
    Console& operator=(const Console&) = delete;

    static Console& Get() {
        static Console instance;
        return instance;
    }

    void ClearLog() { items.clear(); };

    void Log(const std::string& message);
    void LogOnDebug(const std::string& message);
    void Error(const std::string& message);
    void Warn(const std::string& message);
    void Info(const std::string& message);

    void Draw(const char* title, bool* open = nullptr);

protected:
    Console() = default;
    ~Console();

private:
    bool redirectOutput;
    bool autoScroll = true;
    bool scrollToBottom = false;
    std::vector<std::string> items;

};