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
class ComponentArray : public IComponentArray
{
public:
    /**
     * Sets the component of a given entity to a specified value.
     *
     * @param entity The entity for which to set the component.
     * @param component The value of the component to set.
     */
    void setComponent(Entity entity, const T& component)
    {
        if (entity >= components.size()) {
            components.resize(entity + 1);
        }
        components[entity] = component;
    }

    /**
     * Removes the component belonging to an entity.
     * @param entity Entity whose component should be removed.
     */
    void removeComponent(Entity entity)
    {
        if (!hasComponent(entity)) {
            Console::get().error(
                "[ComponentArray::removeComponent] Entity ID '" + std::to_string(entity) +
                "' does not have this component."
            );
        }
        components[entity].reset();
    }

    /**
     * Reports whether an entity has a component in this array.
     */
    bool hasComponent(Entity entity) const
    {
        return entity < components.size() && components[entity].has_value();
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
                "[ComponentArray::getComponent] Entity ID '" + std::to_string(entity) +
                "' does not have this component."
            );
        }
        return *components[entity];
    }

    const T& getComponent(Entity entity) const
    {
        if (!hasComponent(entity)) {
            Console::get().error(
                "[ComponentArray::getComponent] Entity ID '" + std::to_string(entity) +
                "' does not have this component."
            );
        }
        return *components[entity];
    }

    void removeEntity(Entity entity) override
    {
        if (entity < components.size()) {
            components[entity].reset();
        }
    }

private:
    std::vector<std::optional<T>> components;
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
     * @return A pointer to the ComponentArray of type T.
     */
    template<class T>
    ComponentArray<T>* getComponentArray()
    {
        const std::type_index typeIdx = typeid(T);
        const auto component = components.find(typeIdx);
        if (component == components.end()) {
            Console::get().error(
                "[ComponentManager::getComponentArray] Component array of type '" +
                std::string(typeIdx.name()) + "' is not registered."
            );
        }
        return static_cast<ComponentArray<T>*>(component->second.get());
    }

    template<class T>
    const ComponentArray<T>* getComponentArray() const
    {
        const std::type_index typeIdx = typeid(T);
        const auto component = components.find(typeIdx);
        if (component == components.end()) {
            Console::get().error(
                "[ComponentManager::getComponentArray] Component array of type '" +
                std::string(typeIdx.name()) + "' is not registered."
            );
        }
        return static_cast<const ComponentArray<T>*>(component->second.get());
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
        components.emplace(typeIdx, std::make_unique<ComponentArray<T>>());
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
        return static_cast<const ComponentArray<T>*>(component->second.get())->hasComponent(entity);
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
