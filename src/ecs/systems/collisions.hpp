#pragma once

#include "../system_manager.hpp"

#include "../entity_manager.hpp"
#include "../component_manager.hpp"

class Collisions : public IUpdateSystem {
private:
    EntityManager* entityManager;
    ComponentManager* componentManager;
public:
    Collisions() : entityManager(EntityManager::GetInstance()), componentManager(ComponentManager::GetInstance()) {}
    
    void Update(float deltaTime) override;
};