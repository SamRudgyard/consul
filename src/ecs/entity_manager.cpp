#include "entity_manager.hpp"

#include "utils/utils.hpp"

EntityManager* EntityManager::instance = nullptr;

Entity EntityManager::CreateEntity() {
    EntityContainer entityContainer;
    entityContainer.entity = entities.size();
    entityContainer.mask = ComponentMask();
    entities.push_back(entityContainer);
    return entityContainer.entity;
}

void EntityManager::DestroyEntity(Entity entity) {
    entities.erase(entities.begin() + entity);
}