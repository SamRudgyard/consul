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
    vector<T> components;
public:
    void SetComponent(Entity entity, T component) {
        if (entity >= components.size()) {
            components.resize(entity + 1);
        }
        components[entity] = component;
    }

    T& GetComponent(Entity entity) {
        if (entity >= components.size()) Error("[ComponentArray::GetComponent] Entity ID '" + to_string(entity) + "' out of bounds for component array of size '" + to_string(components.size()) + "'");
        return components[entity];
    }
};

class ComponentManager {
private:
    static ComponentManager* instance;
    static unsigned int nComponents;
    static inline unordered_map<type_index, unique_ptr<IComponentArray>> components;
    static inline unordered_map<type_index, unsigned int> typeToID;

    template<class T>
    static ComponentArray<T>* GetComponentArray() {
        type_index typeIdx = typeid(T);
        string name = typeIdx.name();
        Log("[ComponentManager::GetComponentArray] Getting component array of type '" + name + "'");
        if (components.find(typeIdx) == components.end()) Error("[ComponentManager::GetComponentArray] Component array of type '" + name + "' not registered.");
        Log("[ComponentManager::GetComponentArray] Got component array of type '" + name + "'");
        return static_cast<ComponentArray<T>*>(components[typeIdx].get());
    }
public:
    ComponentManager() = default;
    ~ComponentManager() = default;

    static ComponentManager* GetInstance() {
        if (instance == nullptr) instance = new ComponentManager();
        return instance;
    }

    template <class T>
    static unsigned int GetComponentID() {
        const type_index typeIdx = typeid(T);
    
        // If not registered, register now
        if (typeToID.find(typeIdx) == typeToID.end()) {
            Log("[ComponentManager::GetComponentID] Registering new component of type '" + string(typeid(T).name()) + "'");
            unsigned int newID = nComponents++;
            typeToID[typeIdx] = newID;
            components[typeIdx] = make_unique<ComponentArray<T>>();  // Register the component array here
        }

        return typeToID[typeIdx];
    }

    template<class T>
    static void AddComponent(Entity entity, const T& component) {
        GetComponentArray<T>()->SetComponent(entity, component);
    }

    template <class T>
    static T& GetComponent(Entity entity) {
        return GetComponentArray<T>()->GetComponent(entity);
    }
};