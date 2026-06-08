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
    std::vector<T> components;
public:
    void SetComponent(Entity entity, T component) {
        if (entity >= components.size()) {
            components.resize(entity + 1);
        }
        components[entity] = component;
    }

    T& GetComponent(Entity entity) {
        if (entity >= components.size()) {
            Console::get().error("[ComponentArray::GetComponent] Entity ID '" + std::to_string(entity) + "' out of bounds for component array of size '" + std::to_string(components.size()) + "'");
        }
        return components[entity];
    }
};

class ComponentManager {
private:
    unsigned int nComponents;
    std::unordered_map<std::type_index, std::unique_ptr<IComponentArray>> components;
    std::unordered_map<std::type_index, unsigned int> typeToID;

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

    template <typename T>
    void RegisterComponent() {
        const std::type_index typeIdx = typeid(T);
        if (components.find(typeIdx) == components.end()) {
            components[typeIdx] = std::make_unique<ComponentArray<T>>();
        }
    }

    template <class T>
    unsigned int GetComponentID() {
        const std::type_index typeIdx = typeid(T);
        auto it = typeToID.find(typeIdx);
        if (it != typeToID.end()) return it->second;

        unsigned int newID = nComponents++;
        typeToID[typeIdx] = newID;
        return newID;
    }

    template<class T>
    void AddComponent(Entity entity, const T& component) {
        GetComponentArray<T>()->SetComponent(entity, component);
    }

    template <class T>
    T& GetComponent(Entity entity) {
        return GetComponentArray<T>()->GetComponent(entity);
    }
};