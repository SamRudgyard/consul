#pragma once

#include <string>
#include <vector>

#define MAX_CONSOLE_SIZE 1000

/**
 * Minimal in-application console with a rolling log.
 */
class Console
{
public:
    Console(const Console&) = delete;
    Console& operator=(const Console&) = delete;

    /**
     * Get the singleton Console instance.
     * @returns The single Console instance.
     */
    static Console& get() {
        static Console instance;
        return instance;
    }

    /**
     * Clear all log items.
     */
    void clearLog() { items.clear(); };

    /**
     * Returns the current log entries.
     */
    const std::vector<std::string>& getItems() const { return items; }

    /**
     * Appends a standard message to the log.
     */
    void log(const std::string& message);

    /**
     * Appends a message to the log on non-release builds.
     */
    void logOnDebug(const std::string& message);

    /**
     * Appends an error message.
     * @throws `std::runtime_error` after logging.
     */
    void error(const std::string& message);
    
    /**
     * Appends a warning message to the log.
     */
    void warn(const std::string& message);

    /**
     * Appends an info message to the log.
     */
    void info(const std::string& message);

protected:
    Console() = default;
    ~Console();

private:
    bool redirectOutput;
    std::vector<std::string> items;

};
