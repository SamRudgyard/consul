#pragma once

#include <cstddef>
#include <vector>

struct ProfileEvent
{
    std::size_t methodIndex = 0;
    float durationSeconds = 0.0f;
};

class ProfileEventQueue
{
public:
    static void pushEvent(const ProfileEvent& event);
    static void drainEvents(std::vector<ProfileEvent>& outEvents);
};
