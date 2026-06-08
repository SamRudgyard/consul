#pragma once

#include <vector>
#include <bitset>
#include <memory>
#include <typeindex>
#include <unordered_map>

#include "ecs_types.hpp"
#include "core/console/console.hpp"

class IComponentArray {
public:
    virtual ~IComponentArray() = default;
};

template<class T>
class ComponentArray : public IComponentArray {
private:
    std::vector<T> components; // Array of components, where the index corresponds to the entity
public:
    /**
     * Sets the component of a given entity to a specified value.
     *
     * @param entity The entity for which to set the component.
     * @param component The value of the component to set.
     */
    void SetComponent(Entity entity, T component) {
        if (entity >= components.size()) {
            components.resize(entity + 1);
        }
        components[entity] = component;
    }

    /**
     * Gets the component of a given entity.
     *
     * @param entity The entity for which to retrieve the component.
     * @return The component of the entity.
     */
    T& GetComponent(Entity entity) {
        if (entity >= components.size()) {
            Console::get().error("[ComponentArray::GetComponent] Entity ID '" + std::to_string(entity) + "' out of bounds for component array of size '" + std::to_string(components.size()) + "'");
        }
        return components[entity];
    }
};

class ComponentManager {
private:
    std::unordered_map<std::type_index, std::unique_ptr<IComponentArray>> components;
    std::unordered_map<std::type_index, unsigned int> typeToID;

    /**
     * Gets the component array of a specified type.
     *
     * @tparam T The type of the component.
     * @return A pointer to the ComponentArray of type T.
     */
    template<class T>
    ComponentArray<T>* GetComponentArray() {
        type_index typeIdx = typeid(T);
        string name = typeIdx.name();
        if (components.find(typeIdx) == components.end()) {
            Console::get().error("[ComponentManager::GetComponentArray] Component array of type '" + name + "' not registered.");
        }
        return static_cast<ComponentArray<T>*>(components[typeIdx].get());
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
    unsigned int GetComponentID() {
        const std::type_index typeIdx = typeid(T);

        // If not registered, register now
        if (typeToID.find(typeIdx) == typeToID.end()) {
            Console::get().logOnDebug("[ComponentManager::GetComponentID] Registering new component of type '" + std::string(typeid(T).name()) + "'");
            unsigned int newID = typeToID.size();
            typeToID[typeIdx] = newID;
            components[typeIdx] = std::make_unique<ComponentArray<T>>();
        }
        return typeToID[typeIdx];
    }

    /**
     * Adds a component of type T to the specified entity (with specified value).
     *
     * @tparam T The type of the component to add.
     * @param entity The entity to add the component to.
     * @param component The value of the component to add.
     */
    template<class T>
    void AddComponent(Entity entity, const T& component) {
        GetComponentArray<T>()->SetComponent(entity, component);
    }

    /**
     * Gets a component of type T from the specified entity.
     *
     * @tparam T The type of the component to retrieve.
     * @param entity The entity to add the component to.
     * 
     * @return The component of type T from the specified entity.
     */
    template <class T>
    T& GetComponent(Entity entity) {
        return GetComponentArray<T>()->GetComponent(entity);
    }
};