#pragma once

#include "SDL3/SDL.h"
#include "glm/glm.hpp"

#include "ecs/entity_manager.hpp"
#include "ecs/component_manager.hpp"
#include "ecs/system_manager.hpp"
#include "ecs/app_manager.hpp"

using namespace glm;

struct Timer {
    float timeElapsedSecs = 0.f;
    uint64_t prevTime = SDL_GetPerformanceCounter();

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
    void SetUpEntities();

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    Timer timer;
    EntityManager* entityManager = EntityManager::GetInstance();
    ComponentManager* componentManager = ComponentManager::GetInstance();
    SystemManager* systemManager = SystemManager::GetInstance();
    AppManager* appManager = AppManager::GetInstance();

    float deltaTime = 0.;
    int targetFPS = 60;
};