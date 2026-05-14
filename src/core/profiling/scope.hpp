#pragma once

#include <chrono>
#include <cstddef>

class Scope
{
public:
    static constexpr std::size_t INVALID_METHOD_INDEX = (std::size_t)(-1);

    Scope() = default;
    explicit Scope(std::size_t methodIndex);
    Scope(const Scope&) = delete;
    Scope& operator=(const Scope&) = delete;
    Scope(Scope&& other) noexcept;
    ~Scope();

private:
    std::size_t methodIndex = INVALID_METHOD_INDEX;
    std::chrono::steady_clock::time_point startTime;
};
