#include "entity_manager.hpp"

#include <utility>

EntityManager::EntityManager(std::shared_ptr<ComponentManager> componentManager)
    : componentManager(std::move(componentManager))
{
    if (!this->componentManager) {
        Console::get().error("[EntityManager::EntityManager] Component manager is null.");
    }
}

Entity EntityManager::createEntity()
{
    if (entityCount >= MAX_ENTITIES) {
        Console::get().error(
            "[EntityManager::createEntity] Cannot create more than " +
            std::to_string(MAX_ENTITIES) + " entities."
        );
    }

    Entity entity;
    if (availableEntities.empty()) {
        entity = static_cast<Entity>(entities.size());
        entities.push_back({entity, ComponentMask{}, true});
    } else {
        entity = availableEntities.front();
        availableEntities.pop();
        entities[entity] = {entity, ComponentMask{}, true};
    }

    entityCount++;
    return entity;
}

void EntityManager::destroyEntity(Entity entity)
{
    validateEntity(entity);

    componentManager->entityDestroyed(entity);
    entities[entity].mask.reset();
    entities[entity].isAlive = false;
    availableEntities.push(entity);
    entityCount--;
}

bool EntityManager::isAlive(Entity entity) const
{
    return entity < entities.size() && entities[entity].isAlive;
}

void EntityManager::validateEntity(Entity entity) const
{
    if (!isAlive(entity)) {
        Console::get().error(
            "[EntityManager] Entity ID '" + std::to_string(entity) + "' is not alive."
        );
    }
}
