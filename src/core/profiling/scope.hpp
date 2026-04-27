#pragma once

#include <chrono>
#include <cstddef>

class Profiler;

class Scope
{
public:
    static constexpr std::size_t INVALID_METHOD_INDEX = static_cast<std::size_t>(-1);

    Scope() = default;
    Scope(Profiler* profiler, std::size_t methodIndex);
    Scope(const Scope&) = delete;
    Scope& operator=(const Scope&) = delete;
    Scope(Scope&& other) noexcept;
    Scope& operator=(Scope&& other) noexcept;
    ~Scope();

    std::size_t getMethod() const { return methodIndex; }
    float getDurationSeconds() const { return durationSeconds; }

private:
    Profiler* profiler = nullptr;
    std::size_t methodIndex = INVALID_METHOD_INDEX;
    std::chrono::steady_clock::time_point startTime;
    float durationSeconds = 0.0f;
};
