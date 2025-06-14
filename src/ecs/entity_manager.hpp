#pragma once

#include <bitset>
#include <cstdint>
#include <vector>

#include "component_manager.hpp"

using namespace std;

using Entity = uint32_t;
const Entity MAX_ENTITIES = 5000;
using ComponentType = uint32_t;
const ComponentType MAX_COMPONENTS = 32;
typedef bitset<MAX_COMPONENTS> ComponentMask;

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
     * Adds a component of type T to the specified entity.
     *
     * @tparam T The type of the component to be added.
     * @param entity The entity from which the component is to be added.
     */
    template<typename T>
    void Add(Entity entity) {
        unsigned int componentID = ComponentManager::Get<T>();
        entities[entity].mask.set(componentID);
    }

    /**
     * Removes a component of type T from the specified entity.
     *
     * @tparam T The type of the component to be removed.
     * @param entity The entity from which the component is to be removed.
     */
    template<typename T>
    void Remove(Entity entity) {
        unsigned int componentID = ComponentManager::Get<T>();
        entities[entity].mask.reset(componentID);
    }
};