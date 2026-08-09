#include "ecs.hpp"

Entity ECS::createEntity()
{
    if (entityCount >= MAX_ENTITIES) {
        Console::get().error("[ECS::createEntity] Cannot create more than " + std::to_string(MAX_ENTITIES) + " entities.");
    }

    Entity entity;

    if (!availableEntities.empty()) {
        entity = availableEntities.back();
        availableEntities.pop_back();

        entities[entity] = EntityContainer{entity, {}, true};
    } else {
        entity = static_cast<Entity>(entities.size());

        if (entities.size() >= MAX_ENTITIES) {
            Console::get().error("[ECS::createEntity] Entity slot limit reached.");
        }

        entities.push_back(EntityContainer{entity, {}, true});
    }

    entityCount++;

    return entity;
}

void ECS::destroyEntity(Entity entity)
{
    validateEntity(entity);

    for (auto& [type, componentSet] : components) {
        const ComponentType componentID = typeToID.at(type);

        if (entities[entity].mask.test(componentID)) {
            componentSet->removeComponent(entity);
        }
    }

    entities[entity].mask.reset();
    entities[entity].isAlive = false;

    availableEntities.push_back(entity);

    entityCount--;
}

bool ECS::isAlive(Entity entity) const noexcept
{
    return entity < entities.size() && entities[entity].isAlive;
}

void ECS::validateEntity(Entity entity) const
{
    if (isAlive(entity)) {
        return;
    }

    Console::get().error("[ECS] Entity ID '" + std::to_string(entity) + "' is not alive.");
}
