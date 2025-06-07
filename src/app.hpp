#pragma once

#include "SDL3/SDL.h"
#include "glm/glm.hpp"

using namespace glm;

enum class AppState {
    Startup,
    Loading,
    Menu,
    Paused,
    Running,
    GameOver,
    Quitting
};

struct Timer {
    float timeElapsedSecs = 0.f;
    uint64_t ticks = 0;

    void Tick() {
        const uint64_t now = SDL_GetPerformanceCounter();
        const uint64_t delta = now - ticks;
        static const uint64_t ticksPerSec = SDL_GetPerformanceFrequency();
        timeElapsedSecs = delta/static_cast<float>(ticksPerSec);
        ticks = now;
    }
};

class App {
public:
    App() = default;
    ~App();
    void Init(const char* title, int x, int y, int width, int height, Uint32 flags);
    void Run();

    AppState state = AppState::Startup;
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    Timer timer;
    float deltaTime = 0.;
    int targetFPS = 30;

    vec2 paddlePos;
    vec2 ballPos;
};