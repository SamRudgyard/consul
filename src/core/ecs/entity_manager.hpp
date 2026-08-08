#pragma once

#include <cstddef>
#include <memory>
#include <queue>
#include <string>
#include <typeinfo>
#include <vector>

#include "component_manager.hpp"
#include "ecs_types.hpp"

struct EntityContainer
{
    Entity entity = 0;
    ComponentMask mask;
    bool isAlive = false;
};

/**
 * Manages entity lifetimes and component signatures. Component values are
 * stored by the associated ComponentManager and accessed through this class.
 */
class EntityManager
{
public:
    explicit EntityManager(std::shared_ptr<ComponentManager> componentManager);
    ~EntityManager() = default;

    /**
     * Retrieves the vector of entities.
     *
     * This function returns a reference to the vector of entities managed by
     * the EntityManager.
     *
     * @return A const reference to the entity slots.
     */
    const std::vector<EntityContainer>& getEntities() const { return entities; }

    /**
     * Gets the number of currently living entities.
     */
    std::size_t getEntityCount() const { return entityCount; }

    /**
     * Reports whether an entity ID currently refers to a living entity.
     */
    bool isAlive(Entity entity) const;
    
    /**
     * Creates a new entity.
     *
     * This function will create a new entity with the smallest possible ID and
     * return that ID. If the entity ID is already in use, a new one is found.
     *
     * @return The ID of the newly created entity.
     */
    Entity createEntity();

    /**
     * Destroys an entity.
     *
     * This function will remove the specified entity from the manager and reclaim
     * its ID.
     *
     * @param entity The ID of the entity to be destroyed.
     */
    void destroyEntity(Entity entity);

    /**
     * Adds a component of type T to the specified entity (with default value).
     *
     * @param entity The ID of the entity to add the component to.
     */
    template<class T>
    void addComponent(Entity entity)
    {
        addComponent<T>(entity, T{});
    }

    /**
     * Adds a component of type T to the specified entity (with specified value).
     *
     * @param entity The ID of the entity to add the component to.
     * @param component The value of the component to add.
     */
    template<class T>
    void addComponent(Entity entity, const T& component)
    {
        validateEntity(entity);

        const ComponentType componentID = componentManager->getComponentID<T>();
        componentManager->addComponent<T>(entity, component);
        entities[entity].mask.set(componentID);
    }

    /**
     * Removes a component of type T from the specified entity.
     *
     * @tparam T The type of the component to remove.
     * @param entity The ID of the entity from which to remove the component.
     */
    template<class T>
    void removeComponent(Entity entity)
    {
        validateEntity(entity);
        if (!componentManager->hasComponent<T>(entity)) {
            Console::get().error(
                "[EntityManager::removeComponent] Entity ID '" + std::to_string(entity) +
                "' does not have component type '" + std::string(typeid(T).name()) + "'."
            );
        }

        const ComponentType componentID = componentManager->getComponentID<T>();
        componentManager->removeComponent<T>(entity);
        entities[entity].mask.reset(componentID);
    }

    /**
     * Reports whether a living entity has a component of type T.
     */
    template<class T>
    bool hasComponent(Entity entity) const
    {
        validateEntity(entity);
        return componentManager->hasComponent<T>(entity);
    }

    /**
     * Gets a component belonging to a living entity.
     */
    template<class T>
    T& getComponent(Entity entity)
    {
        validateEntity(entity);
        return componentManager->getComponent<T>(entity);
    }

    template<class T>
    const T& getComponent(Entity entity) const
    {
        validateEntity(entity);
        return static_cast<const ComponentManager&>(*componentManager).getComponent<T>(entity);
    }

    /**
     * Collects all entities with a given set of components.
     *
     * @tparam Components The components to query for.
     * @return The entities that have all the given components.
     */
    template<class... Components>
    std::vector<Entity> view() const
    {
        std::vector<Entity> result;
        ComponentMask mask;

        ((mask.set(componentManager->getComponentID<Components>())), ...);
        for (const auto& entityContainer : entities) {
            if (!entityContainer.isAlive) {
                continue;
            }
            if ((entityContainer.mask & mask) != mask) {
                continue;
            }

            result.push_back(entityContainer.entity);
        }
        return result;
    }

    /**
     * Invokes a callback for each matching entity. The callback receives the
     * entity ID followed by mutable references to the requested components.
     *
     * @tparam Components The component types to query for.
     * @tparam Func The type of the callback function.
     * @param func The callback function to invoke for each matching entity.
     */
    template<class... Components, class Func>
    void forEach(Func&& func)
    {
        ComponentMask mask;
        ((mask.set(componentManager->getComponentID<Components>())), ...);

        for (const auto& entityContainer : entities) {
            if (!entityContainer.isAlive) {
                continue;
            }
            if ((entityContainer.mask & mask) != mask) {
                continue;
            }

            Entity entity = entityContainer.entity;
            func(entity, componentManager->getComponent<Components>(entity)...);
        }
    }

private:
    std::vector<EntityContainer> entities;
    std::queue<Entity> availableEntities;
    std::shared_ptr<ComponentManager> componentManager;
    std::size_t entityCount = 0;

    void validateEntity(Entity entity) const;
};
