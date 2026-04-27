#pragma once

#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

class Scope;

class Profiler
{
public:
    static constexpr float MAX_SECONDS_RECORDED = 60.0f;
    static constexpr std::size_t INVALID_METHOD_INDEX = (std::size_t)(-1);

    Profiler() = default;
    Profiler(const Profiler&) = delete;
    Profiler& operator=(const Profiler&) = delete;

    void beginFrame(float frameDeltaSeconds);

    std::size_t registerMethod(const char* methodName);
    void addSample(const Scope& scope);

    Scope profileScope(std::size_t methodIndex);
    Scope profileScope(const char* methodName);

    const std::vector<std::string>& getRegisteredMethods() const { return registeredMethods; }
    const std::vector<float>& getFrameDurations() const { return frameDurations; }
    float getTotalRecordedTime() const { return totalRecordedTime; }
    const std::vector<float>* getMethodHistoryMs(const std::string& methodName) const;

private:
    std::size_t ensureMethodIndex(const char* methodName);

    std::unordered_map<std::string, std::size_t> methodIndices;
    std::vector<std::string> registeredMethods;
    std::vector<std::vector<float>> methodHistoryMs;
    std::vector<float> currentFrameTotalsMs;

    std::vector<float> frameDurations;
    float totalRecordedTime = 0.0f;
    bool hasActiveFrame = false;
};
