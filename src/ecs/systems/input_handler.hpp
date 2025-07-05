#pragma once

#include "../entity_manager.hpp"
#include "../component_manager.hpp"
#include "../system_manager.hpp"

#include "SDL3/SDL.h"
#include "glm/glm.hpp"

using namespace glm;

class InputHandler : public IUpdateSystem {
private:
    EntityManager* entityManager;
    ComponentManager* componentManager;
public:
    InputHandler() : entityManager(EntityManager::GetInstance()), componentManager(ComponentManager::GetInstance()) {};
    void Update(float deltaTime) override;
};