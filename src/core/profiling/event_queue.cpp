#include "event_queue.hpp"

#include <mutex>

static std::mutex gProfileEventQueueMutex;
static std::vector<ProfileEvent> gProfileEvents;

void ProfileEventQueue::pushEvent(const ProfileEvent& event)
{
    std::lock_guard<std::mutex> lock(gProfileEventQueueMutex);
    gProfileEvents.push_back(event);
}

void ProfileEventQueue::drainEvents(std::vector<ProfileEvent>& outEvents)
{
    std::lock_guard<std::mutex> lock(gProfileEventQueueMutex);
    outEvents.swap(gProfileEvents);
}
