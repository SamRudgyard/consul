#pragma once

/**
 * Frame timing utilities. Call NewFrame() once per frame to update timing values.
 */
class Time
{
public:
    static double currentTime;
    static double previousTime;
    static double deltaTime;
    static double frameTime;
    static double targetFrameTime;
    static int frameCount;

    /**
     * Update timing values for a new frame (deltaTime, frameTime, frameCount).
     */
    static void NewFrame();

    /**
     * Set the desired target FPS and compute the corresponding frame time (seconds).
     * @param fps Target frames per second.
     */
    static void SetTargetFPS(unsigned int fps);
};