#pragma once

struct Time
{
    float currentTime = 0.0f;               // Current time in seconds
    float previousTime = 0.0f;              // Time at the previous frame in seconds
    float deltaTime = 0.0f;                 // Time elapsed since last frame in seconds
    float targetFrameTime = 1.0f / 2.0f;    // Target frame time for 60 FPS
    unsigned int frameCount = 0;            // Total number of frames rendered
};