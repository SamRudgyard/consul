#include "scope.hpp"

#include "event_queue.hpp"

Scope::Scope(std::size_t methodIndex)
    : methodIndex(methodIndex),
      startTime(std::chrono::steady_clock::now())
{
}

Scope::Scope(Scope&& other) noexcept
    : methodIndex(other.methodIndex),
      startTime(other.startTime)
{
    other.methodIndex = INVALID_METHOD_INDEX;
}

Scope& Scope::operator=(Scope&& other) noexcept
{
    if (this == &other) {
        return *this;
    }

    methodIndex = other.methodIndex;
    startTime = other.startTime;

    other.methodIndex = INVALID_METHOD_INDEX;

    return *this;
}

Scope::~Scope()
{
    if (methodIndex == INVALID_METHOD_INDEX) {
        return;
    }

    const float durationSeconds = (float)(
        std::chrono::duration<double>(std::chrono::steady_clock::now() - startTime).count()
    );

    ProfileEventQueue::pushEvent(ProfileEvent{
        methodIndex,
        durationSeconds
    });
}
