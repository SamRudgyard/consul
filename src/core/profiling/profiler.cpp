#include "profiler.hpp"

#include <algorithm>

#include "event_queue.hpp"
#include "scope.hpp"
#include "maths/unit_conversions.hpp"

void Profiler::beginFrame(float frameDeltaSeconds)
{
    if (!hasActiveFrame) {
        hasActiveFrame = true;
        return;
    }

    std::vector<ProfileEvent> pendingEvents;
    ProfileEventQueue::drainEvents(pendingEvents);
    for (const ProfileEvent& event : pendingEvents) {
        if (event.durationSeconds < 0.0f || event.methodIndex >= currentFrameTotalsMs.size()) {
            continue;
        }

        currentFrameTotalsMs[event.methodIndex] += event.durationSeconds*SECONDS_TO_MILLISECONDS;
    }

    frameDeltaSeconds = std::max(0.0f, frameDeltaSeconds);

    frameDurations.push_back(frameDeltaSeconds);
    totalRecordedTime += frameDeltaSeconds;

    for (std::size_t methodIdx = 0; methodIdx < currentFrameTotalsMs.size(); methodIdx++) {
        methodHistoryMs[methodIdx].push_back(currentFrameTotalsMs[methodIdx]);
        currentFrameTotalsMs[methodIdx] = 0.0f;
    }

    while (totalRecordedTime > MAX_SECONDS_RECORDED && !frameDurations.empty()) {
        totalRecordedTime -= frameDurations.front();
        frameDurations.erase(frameDurations.begin());

        for (auto& history : methodHistoryMs) {
            history.erase(history.begin());
        }
    }
}

std::size_t Profiler::registerMethod(const char* methodName)
{
    return ensureMethodIndex(methodName);
}

Scope Profiler::profileScope(const char* methodName)
{
    return Scope(ensureMethodIndex(methodName));
}

const std::vector<float>* Profiler::getMethodHistoryMs(const std::string& methodName) const
{
    auto methodIt = methodIndices.find(methodName);
    if (methodIt == methodIndices.end()) {
        return nullptr;
    }

    return &methodHistoryMs[methodIt->second];
}

std::size_t Profiler::ensureMethodIndex(const char* methodName)
{
    if (methodName == nullptr || methodName[0] == '\0') {
        static constexpr const char* UNKNOWN_METHOD_NAME = "<unknown>";
        methodName = UNKNOWN_METHOD_NAME;
    }

    auto existingMethod = methodIndices.find(methodName);
    if (existingMethod != methodIndices.end()) {
        return existingMethod->second;
    }

    const std::size_t newIndex = registeredMethods.size();
    registeredMethods.emplace_back(methodName);
    methodIndices.emplace(registeredMethods.back(), newIndex);

    methodHistoryMs.emplace_back(frameDurations.size(), 0.0f);
    currentFrameTotalsMs.emplace_back(0.0f);

    return newIndex;
}
