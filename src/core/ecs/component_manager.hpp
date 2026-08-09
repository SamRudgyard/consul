#pragma once

#include <memory>
#include <optional>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "core/console/console.hpp"
#include "ecs_types.hpp"

class IComponentArray
{
public:
    virtual ~IComponentArray() = default;

    virtual void removeEntity(Entity entity) = 0;
};

template<class T>
class ComponentSparseSet : public IComponentArray
{
public:
    /**
     * Adds the given component to the specified entity.
     *
     * @param entity The entity for which to set the component.
     * @param component The value of the component to set.
     */
    void addComponent(Entity entity, const T& component)
    {
        if (hasComponent(entity)) {
            Console::get().error(
                "[ComponentSparseSet::addComponent] Entity ID '" + std::to_string(entity) +
                "' already has this component."
            );
            return;
        }

        if (entity >= entitiesToDenseIndices.size()) {
            entitiesToDenseIndices.resize(entity + 1, std::numeric_limits<std::uint32_t>::max());
        }

        if (denseComponents.size() >= MAX_ENTITIES) {
            Console::get().error(
                "[ComponentSparseSet::addComponent] Cannot add more than " +
                std::to_string(MAX_ENTITIES) + " components of this type."
            );
        }

        const std::uint32_t denseIndex = static_cast<std::uint32_t>(denseComponents.size());
        denseComponents.push_back(component);
        denseEntities.push_back(entity);
        entitiesToDenseIndices[entity] = denseIndex;
    }

    /**
     * Removes the component belonging to an entity.
     * @param entity Entity whose component should be removed.
     */
    void removeComponent(Entity entity)
    {
        if (!hasComponent(entity)) {
            Console::get().error(
                "[ComponentSparseSet::removeComponent] Entity ID '" + std::to_string(entity) +
                "' does not have this component."
            );
        }

        const std::uint32_t indexToRemove = entitiesToDenseIndices[entity];
        const std::uint32_t lastIndex = static_cast<std::uint32_t>(denseComponents.size() - 1);

        if (indexToRemove != lastIndex) {
            // Move the last component to the removed component's place
            // NOTE: This ensures our dense vectors remain tightly packed,
            // but removing a component changes the order of iteration.
            // This shouldn't be a problem, but it's something to be aware of.
            denseComponents[indexToRemove] = denseComponents[lastIndex];
            denseEntities[indexToRemove] = denseEntities[lastIndex];
            entitiesToDenseIndices[denseEntities[indexToRemove]] = indexToRemove;
        }

        denseComponents.pop_back();
        denseEntities.pop_back();
        entitiesToDenseIndices[entity] = std::numeric_limits<std::uint32_t>::max();
    }

    /**
     * Reports whether an entity has a component in this array.
     * 
     * @param entity The entity to check for a component.
     * @return True if the entity has a component, false otherwise.
     */
    bool hasComponent(Entity entity) const
    {
        if (entity >= entitiesToDenseIndices.size()) {
            return false;
        }
        const std::uint32_t denseIndex = entitiesToDenseIndices[entity];
        return denseIndex < denseComponents.size() && denseEntities[denseIndex] == entity;
    }

    /**
     * Gets the component of a given entity.
     *
     * @param entity The entity for which to retrieve the component.
     * @return The component of the entity.
     */
    T& getComponent(Entity entity)
    {
        if (!hasComponent(entity)) {
            Console::get().error(
                "[ComponentSparseSet::getComponent] Entity ID '" + std::to_string(entity) +
                "' does not have this component."
            );
        }
        return denseComponents[entitiesToDenseIndices[entity]];
    }

    const T& getComponent(Entity entity) const
    {
        if (!hasComponent(entity)) {
            Console::get().error(
                "[ComponentSparseSet::getComponent] Entity ID '" + std::to_string(entity) +
                "' does not have this component."
            );
        }
        return denseComponents[entitiesToDenseIndices[entity]];
    }

private:
    std::vector<std::uint32_t> entitiesToDenseIndices; // entity ID -> index in dense arrays
    std::vector<T> denseComponents; // index in dense array -> component value
    std::vector<Entity> denseEntities; // index in dense array -> entity ID
};

class ComponentManager
{
private:
    std::unordered_map<std::type_index, std::unique_ptr<IComponentArray>> components;
    std::unordered_map<std::type_index, ComponentType> typeToID;

    /**
     * Gets the component array of a specified type.
     *
     * @tparam T The type of the component.
     * @return A pointer to the ComponentSparseSet of type T.
     */
    template<class T>
    ComponentSparseSet<T>* getComponentArray()
    {
        const std::type_index typeIdx = typeid(T);
        const auto component = components.find(typeIdx);
        if (component == components.end()) {
            Console::get().error(
                "[ComponentManager::getComponentArray] Component array of type '" +
                std::string(typeIdx.name()) + "' is not registered."
            );
        }
        return static_cast<ComponentSparseSet<T>*>(component->second.get());
    }

    template<class T>
    const ComponentSparseSet<T>* getComponentArray() const
    {
        const std::type_index typeIdx = typeid(T);
        const auto component = components.find(typeIdx);
        if (component == components.end()) {
            Console::get().error(
                "[ComponentManager::getComponentArray] Component array of type '" +
                std::string(typeIdx.name()) + "' is not registered."
            );
        }
        return static_cast<const ComponentSparseSet<T>*>(component->second.get());
    }

public:
    ComponentManager() = default;
    ~ComponentManager() = default;

    /**
     * Gets the unique ID for the component type `T`. If this is the first time requesting an ID for this type, a new ID is generated and stored.
     * @tparam T The component type for which to get the ID.
     * @returns The unique ID of the component type `T`.
     */
    template <class T>
    ComponentType getComponentID()
    {
        const std::type_index typeIdx = typeid(T);
        const auto componentID = typeToID.find(typeIdx);
        if (componentID != typeToID.end()) {
            // Already registered, return the existing ID
            return componentID->second;
        }

        if (typeToID.size() >= MAX_COMPONENTS) {
            Console::get().error(
                "[ComponentManager::getComponentID] Cannot register more than " +
                std::to_string(MAX_COMPONENTS) + " component types."
            );
        }

        Console::get().logOnDebug(
            "[ComponentManager::getComponentID] Registering new component of type '" +
            std::string(typeIdx.name()) + "'."
        );

        const ComponentType newID = static_cast<ComponentType>(typeToID.size());
        typeToID.emplace(typeIdx, newID);
        components.emplace(typeIdx, std::make_unique<ComponentSparseSet<T>>());
        return newID;
    }

    /**
     * Adds a component of type T to the specified entity (with specified value).
     *
     * @tparam T The type of the component to add.
     * @param entity The entity to add the component to.
     * @param component The value of the component to add.
     */
    template<class T>
    void addComponent(Entity entity, const T& component)
    {
        getComponentID<T>();
        getComponentArray<T>()->setComponent(entity, component);
    }

    /**
     * Removes a component of type T from an entity.
     */
    template<class T>
    void removeComponent(Entity entity)
    {
        getComponentArray<T>()->removeComponent(entity);
    }

    /**
     * Reports whether an entity has a component of type T.
     */
    template<class T>
    bool hasComponent(Entity entity) const
    {
        const auto component = components.find(typeid(T));
        if (component == components.end()) {
            return false;
        }
        return static_cast<const ComponentSparseSet<T>*>(component->second.get())->hasComponent(entity);
    }

    /**
     * Gets a component of type T from the specified entity.
     *
     * @tparam T The type of the component to retrieve.
     * @param entity The entity from which to retrieve the component.
     * 
     * @return The component of type T from the specified entity.
     */
    template <class T>
    T& getComponent(Entity entity)
    {
        return getComponentArray<T>()->getComponent(entity);
    }

    template <class T>
    const T& getComponent(Entity entity) const
    {
        return getComponentArray<T>()->getComponent(entity);
    }

    /**
     * Removes every component owned by a destroyed entity.
     */
    void entityDestroyed(Entity entity)
    {
        for (auto& component : components) {
            component.second->removeEntity(entity);
        }
    }
};
