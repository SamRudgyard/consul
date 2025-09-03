#pragma once

class Time
{
public:
    static double currentTime;
    static double previousTime;
    static double deltaTime;
    static double frameTime;
    static double targetFrameTime;
    static int frameCount;

    static void NewFrame();
    static void SetTargetFPS(unsigned int fps);
};