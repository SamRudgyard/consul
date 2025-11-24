#include "time.hpp"

#include "core/console/console.hpp"
#include "GLFW/glfw3.h"
#include "utils.hpp"

double Time::currentTime = 0.0;
double Time::previousTime = 0.0;
double Time::deltaTime = 0.0;
double Time::frameTime = 0.0;
double Time::targetFrameTime = 0.0;
int Time::frameCount = 0;

void Time::newFrame()
{
    previousTime = currentTime;
    currentTime = glfwGetTime();
    deltaTime = currentTime - previousTime;
    frameTime += deltaTime;
    frameCount++;

    Console::get().logOnDebug("[Time::newFrame] Frame " + std::to_string(frameCount) + ": " + std::to_string(int(deltaTime*1000)) + " ms (Total time: " + std::to_string(int(currentTime)) + " s)");
}

void Time::setTargetFPS(unsigned int fps)
{
    Console& console = Console::get();

    if (fps < 1) {
        console.error("[Time::setTargetFPS] Invalid target FPS: " + std::to_string(fps));
        return;
    }

    targetFrameTime = double(1.0/fps);

    console.logOnDebug("[Time::setTargetFPS] Target FPS set to " + std::to_string(fps) + " (" + std::to_string(int(targetFrameTime*1000)) + " ms per frame)");
}