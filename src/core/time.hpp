#pragma once

struct Time
{
    double currentTime = 0.0;               // Current time in seconds
    double previousTime = 0.0;              // Time at the previous frame in seconds
    double deltaTime = 0.0;                 // Time elapsed since last frame in seconds
    double targetFrameTime = 1.0 / 60.0;    // Target frame time for 60 FPS
    unsigned int frameCount = 0;            // Total number of frames rendered
};
