#pragma once

#include "../system_manager.hpp"

#include "../entity_manager.hpp"
#include "../component_manager.hpp"

class PhysicsSystem : public IUpdateSystem {
private:
    EntityManager* entityManager;
    ComponentManager* componentManager;
public:
    PhysicsSystem() : entityManager(EntityManager::GetInstance()), componentManager(ComponentManager::GetInstance()) {}

    void Update(float deltaTime) override;
};