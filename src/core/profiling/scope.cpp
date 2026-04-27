#include "scope.hpp"

#include "profiler.hpp"

Scope::Scope(Profiler* profiler, std::size_t methodIndex)
    : profiler(profiler),
      methodIndex(methodIndex),
      startTime(std::chrono::steady_clock::now())
{
}

Scope::Scope(Scope&& other) noexcept
    : profiler(other.profiler),
      methodIndex(other.methodIndex),
      startTime(other.startTime),
      durationSeconds(other.durationSeconds)
{
    other.profiler = nullptr;
    other.methodIndex = INVALID_METHOD_INDEX;
    other.durationSeconds = 0.0f;
}

Scope& Scope::operator=(Scope&& other) noexcept
{
    if (this == &other) {
        return *this;
    }

    profiler = other.profiler;
    methodIndex = other.methodIndex;
    startTime = other.startTime;
    durationSeconds = other.durationSeconds;

    other.profiler = nullptr;
    other.methodIndex = INVALID_METHOD_INDEX;
    other.durationSeconds = 0.0f;

    return *this;
}

Scope::~Scope()
{
    if (profiler == nullptr || methodIndex == INVALID_METHOD_INDEX) {
        return;
    }

    durationSeconds = (float)(
        std::chrono::duration<double>(std::chrono::steady_clock::now() - startTime).count()
    );
    profiler->addSample(*this);
}
