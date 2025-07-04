#pragma once

#include "../system_manager.hpp"

#include "../entity_manager.hpp"
#include "../component_manager.hpp"

class MovementSystem : public IUpdateSystem {
private:
    EntityManager* entityManager;
    ComponentManager* componentManager;
public:
    MovementSystem() : entityManager(EntityManager::GetInstance()), componentManager(ComponentManager::GetInstance()) {}

    void Update(float deltaTime) override;
};