#pragma once

#include <vector>

#include "ecs_types.hpp"

using namespace std;

class EntityManager {
private:
    static EntityManager* instance;

    struct EntityContainer {
        Entity entity; // ID of a given entity
        ComponentMask mask; // Mask of the components of a given entity
    };
    vector<EntityContainer> entities;
protected:
    EntityManager() = default;
    ~EntityManager() = default;
public:
    EntityManager(EntityManager const&) = delete; // Delete copy constructor
    void operator=(EntityManager const&) = delete; // Delete copy assignment

    /**
     * Retrieves the singleton instance of the EntityManager.
     *
     * This function ensures that only one instance of EntityManager exists
     * throughout the application. If the instance does not exist, it creates
     * a new one. Otherwise, it returns the existing instance.
     *
     * @return A pointer to the singleton instance of EntityManager.
     */
    static EntityManager* GetInstance() {
        if (instance == nullptr) {
            instance = new EntityManager();
        }
        return instance;
    }

    /**
     * Retrieves the vector of entities.
     *
     * This function returns a reference to the vector of entities managed by
     * the EntityManager.
     *
     * @return A reference to the vector of entities.
     */
    vector<EntityContainer>& GetEntities() { return entities; }
    
    /**
     * Creates a new entity.
     *
     * This function will create a new entity with the smallest possible ID and
     * return that ID. If the entity ID is already in use, a new one is found.
     *
     * @return The ID of the newly created entity.
     */
    Entity CreateEntity();

    /**
     * Destroys an entity.
     *
     * This function will remove the specified entity from the manager and reclaim
     * its ID.
     *
     * @param entity The ID of the entity to be destroyed.
     */
    void DestroyEntity(Entity entity);

    /**
     * Adds a component of the specified type to the specified entity.
     *
     * @param entity The entity to which the component is to be added.
     * @param componentID The ID of the component to be added.
     */
    void Add(Entity entity, unsigned int componentID) {
        entities[entity].mask.set(componentID);
    }

    /**
     * Removes a component of the specified type from the specified entity.
     *
     * @param entity The entity from which the component is to be removed.
     * @param componentID The ID of the component to be removed.
     */
    template<typename T>
    void Remove(Entity entity, unsigned int componentID) {
        entities[entity].mask.reset(componentID);
    }
};