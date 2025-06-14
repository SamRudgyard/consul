#pragma once

#include <vector>

#include "ecs_types.hpp"
#include "ecs/component_manager.hpp"

using namespace std;

class EntityManager {
private:
    static EntityManager* instance;
    ComponentManager* componentManager = ComponentManager::GetInstance();

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
     * Adds a component of type T to the specified entity (with default value).
     *
     * @param entity The ID of the entity to add the component to.
     */
    template<class T>
    void AddComponent(Entity entity) {
        unsigned int componentID = componentManager->GetComponentID<T>();
        entities[entity].mask.set(componentID);
        componentManager->AddComponent<T>(entity, T());
    }

    /**
     * Adds a component of type T to the specified entity (with specified value).
     *
     * @param entity The ID of the entity to add the component to.
     * @param component The value of the component to add.
     */
    template<class T>
    void AddComponent(Entity entity, const T& component) {
        unsigned int componentID = componentManager->GetComponentID<T>();
        entities[entity].mask.set(componentID);
        componentManager->AddComponent<T>(entity, component);
    }

    /**
     * Removes a component of type T from the specified entity.
     *
     * @tparam T The type of the component to remove.
     * @param entity The ID of the entity from which to remove the component.
     */

    template<typename T>
    void RemoveComponent(Entity entity) {
        unsigned int componentID = componentManager->GetComponentID<T>();
        entities[entity].mask.reset(componentID);
    }
};