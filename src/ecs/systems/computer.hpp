#pragma once

#include "../system_manager.hpp"

#include "../entity_manager.hpp"
#include "../component_manager.hpp"

class ComputerSystem : public IRunUpdateSystem{
private:
    EntityManager* entityManager;
    ComponentManager* componentManager;
public:
    ComputerSystem() : entityManager(EntityManager::GetInstance()), componentManager(ComponentManager::GetInstance()) {}
    void Update(float deltaTime) override;
};