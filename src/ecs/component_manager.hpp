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
    static inline unordered_map<std::type_index, std::unique_ptr<IComponentArray>> components;
    static inline unordered_map<std::type_index, unsigned int> typeToID;

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

    static ComponentManager* GetInstance() {
        if (instance == nullptr) instance = new ComponentManager();
        return instance;
    }

    template <typename T>
    static void RegisterComponent() {
        const std::type_index typeIdx = typeid(T);
        if (components.find(typeIdx) == components.end()) {
            components[typeIdx] = std::make_unique<ComponentArray<T>>();
        }
    }

    template <class T>
    static unsigned int GetComponentID() {
        const std::type_index typeIdx = typeid(T);
        auto it = typeToID.find(typeIdx);
        if (it != typeToID.end()) return it->second;

        unsigned int newID = nComponents++;
        typeToID[typeIdx] = newID;
        return newID;
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