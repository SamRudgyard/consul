#pragma once

#include "glm/glm.hpp"

#include "../system_manager.hpp"

#include "../entity_manager.hpp"
#include "../component_manager.hpp"

using namespace glm;

class Collisions : public IUpdateSystem {
private:
    EntityManager* entityManager;
    ComponentManager* componentManager;
public:
    Collisions() : entityManager(EntityManager::GetInstance()), componentManager(ComponentManager::GetInstance()) {}
    
    void Update(float deltaTime) override;
    bool HasCollidedAABB(Entity entityA, Entity entityB, vec2& collisionNormal);
};