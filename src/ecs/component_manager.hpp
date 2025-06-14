#pragma once

#include <vector>
#include <bitset>
#include <typeindex>
#include <unordered_map>

#include "ecs_types.hpp"

#include "utils/utils.hpp"

using namespace std;

class IComponentArray {
public:
    virtual ~IComponentArray() = default;
};

template<class T>
class ComponentArray : public IComponentArray {
private:
    vector<T> components; // Array of components, where the index corresponds to the entity
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
        if (entity >= components.size()) Error("[ComponentArray::GetComponent] Entity ID '" + to_string(entity) + "' out of bounds for component array of size '" + to_string(components.size()) + "'");
        return components[entity];
    }
};

class ComponentManager {
private:
    static ComponentManager* instance;
    static inline unordered_map<type_index, unique_ptr<IComponentArray>> components; // Map of component arrays, i.e. each component type has its own array
    static inline unordered_map<type_index, unsigned int> typeToID;

    /**
     * Gets the component array of a specified type.
     *
     * @tparam T The type of the component.
     * @return A pointer to the ComponentArray of type T.
     */

    template<class T>
    static ComponentArray<T>* GetComponentArray() {
        type_index typeIdx = typeid(T);
        string name = typeIdx.name();
        if (components.find(typeIdx) == components.end()) Error("[ComponentManager::GetComponentArray] Component array of type '" + name + "' not registered.");
        return static_cast<ComponentArray<T>*>(components[typeIdx].get());
    }
public:
    ComponentManager() = default;
    ~ComponentManager() = default;

    /**
     * Retrieves the singleton instance of the ComponentManager.
     *
     * @return A pointer to the singleton instance of ComponentManager.
     */
    static ComponentManager* GetInstance() {
        if (instance == nullptr) instance = new ComponentManager();
        return instance;
    }

    /**
     * Retrieves the unique ID of a component type.
     *
     * @note This function is thread-safe.
     *
     * @tparam T The type of the component.
     * @return The unique ID of the component type.
     */
    template <class T>
    static unsigned int GetComponentID() {
        const type_index typeIdx = typeid(T);
    
        // If not registered, register now
        if (typeToID.find(typeIdx) == typeToID.end()) {
            Log("[ComponentManager::GetComponentID] Registering new component of type '" + string(typeid(T).name()) + "'");
            unsigned int newID = typeToID.size();
            typeToID[typeIdx] = newID;
            components[typeIdx] = make_unique<ComponentArray<T>>();  // Register the component array here
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
    static void AddComponent(Entity entity, const T& component) {
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
    static T& GetComponent(Entity entity) {
        return GetComponentArray<T>()->GetComponent(entity);
    }
};