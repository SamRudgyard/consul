#pragma once

#include "SDL3/SDL.h"
#include "glm/glm.hpp"

#include "ecs/entity_manager.hpp"
#include "ecs/component_manager.hpp"

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
    uint64_t prevTime = 0;

    void Tick() {
        const uint64_t currentTime = SDL_GetPerformanceCounter();
        timeElapsedSecs = (currentTime - prevTime)/(float)SDL_GetPerformanceFrequency();
        prevTime = currentTime;
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
    EntityManager* entityManager = EntityManager::GetInstance();
    ComponentManager* componentManager = ComponentManager::GetInstance();

    float deltaTime = 0.;
    int targetFPS = 60;

    vec2 paddlePos;
    vec2 ballPos;
};