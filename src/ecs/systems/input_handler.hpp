#pragma once

#include "../entity_manager.hpp"
#include "../component_manager.hpp"
#include "../system_manager.hpp"
#include "../app_manager.hpp"

#include "SDL3/SDL.h"
#include "glm/glm.hpp"

using namespace glm;

class InputHandler : public IPauseUpdateSystem {
private:
    EntityManager* entityManager;
    ComponentManager* componentManager;
    AppManager* appManager;

public:
    InputHandler() : entityManager(EntityManager::GetInstance()), componentManager(ComponentManager::GetInstance()), appManager(AppManager::GetInstance()) {};
    void Update(float deltaTime) override;
    void DetectPlayerMovement();
    void DetectTogglePause();
};