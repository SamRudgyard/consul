#include "time.hpp"

#include "GLFW/glfw3.h"
#include "utils.hpp"

double Time::currentTime = 0.0;
double Time::previousTime = 0.0;
double Time::deltaTime = 0.0;
double Time::frameTime = 0.0;
double Time::targetFrameTime = 0.0;
int Time::frameCount = 0;

void Time::NewFrame()
{
    previousTime = currentTime;
    currentTime = glfwGetTime();
    deltaTime = currentTime - previousTime;
    frameTime += deltaTime;
    frameCount++;
}

void Time::SetTargetFPS(unsigned int fps)
{
    if (fps < 1) Error("[ENGINE] Invalid target FPS: " + std::to_string(fps));

    targetFrameTime = double(1.0/fps);

    Log("[ENGINE] Target FPS set to " + std::to_string(fps) + " (" + std::to_string(int(targetFrameTime)*1000) + " ms per frame)");
}