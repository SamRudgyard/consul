#pragma once

#include <bitset>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <limits>
#include <memory>
#include <optional>
#include <queue>
#include <stdexcept>
#include <string>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <utility>
#include <vector>

#include "core/console/console.hpp"

// Adjust these to your existing values.
inline constexpr std::size_t MAX_ENTITIES   = 5000;
inline constexpr std::size_t MAX_COMPONENTS = 64;

using Entity = std::uint32_t;
using ComponentType = std::size_t;
using ComponentMask = std::bitset<MAX_COMPONENTS>;

/**
 * Interface for component sparse sets.
 *
 * ECS needs this so that it can store ComponentSparseSet<Position>,
 * ComponentSparseSet<Velocity>, etc. in the same container.
 */
class IComponentSparseSet
{
public:
    virtual ~IComponentSparseSet() = default;

    /**
     * Removes an entity from this sparse set if present.
     * Used when destroying an entity without knowing its component types.
     * 
     * @param entity The entity to remove.
     */
    virtual void removeEntity(Entity entity) = 0;

    /**
     * Number of components stored in the dense set.
     * 
     * @return The number of components in the dense set.
     */
    [[nodiscard]]
    virtual std::size_t size() const noexcept = 0;

    /**
     * Dense entity list.
     * Exposed through the base class so queries can iterate the smallest
     * relevant component set.
     * 
     * @return The dense entity list.
     */
    [[nodiscard]]
    virtual const std::vector<Entity>& getEntities() const noexcept = 0;
};

/**
 * Sparse-set storage for one component type.
 */
template<class T>
class ComponentSparseSet final : public IComponentSparseSet
{
public:
    using DenseIndex = std::uint32_t;

    static constexpr DenseIndex INVALID_INDEX =
        std::numeric_limits<DenseIndex>::max();

    ComponentSparseSet() = default;
    ~ComponentSparseSet() override = default;

    /**
     * Adds a component by copying it.
     * 
     * @param entity The entity to add the component to.
     * @param component The component to add.
     * @return A reference to the newly added component.
     */
    T& addComponent(Entity entity, const T& component)
    {
        return emplaceComponent(entity, component);
    }

    /**
     * Adds a component by moving it.
     * 
     * @param entity The entity to add the component to.
     * @param component The component to add.
     * @return A reference to the newly added component.
     */
    T& addComponent(Entity entity, T&& component)
    {
        return emplaceComponent(entity, std::move(component));
    }

    /**
     * Constructs a component directly in the dense component array.
     * 
     * @param entity The entity to add the component to.
     * @param args The arguments to forward to the component's constructor.
     * @return A reference to the newly added component.
     */
    template<class... Args>
    T& emplaceComponent(Entity entity, Args&&... args)
    {
        if (hasComponent(entity)) {
            Console::get().error(
                "[ComponentSparseSet::emplaceComponent] Entity ID '" +
                std::to_string(entity) +
                "' already has this component."
            );
        }

        if (denseComponents.size() >= MAX_ENTITIES) {
            Console::get().error(
                "[ComponentSparseSet::emplaceComponent] Cannot add more than " +
                std::to_string(MAX_ENTITIES) +
                " components of this type."
            );
        }

        if (entity >= entitiesToDenseIndices.size()) {
            entitiesToDenseIndices.resize(static_cast<std::size_t>(entity) + 1, INVALID_INDEX);
        }

        const DenseIndex denseIndex = static_cast<DenseIndex>(denseComponents.size());

        denseComponents.emplace_back(std::forward<Args>(args)...);
        denseEntities.push_back(entity);
        entitiesToDenseIndices[entity] = denseIndex;

        return denseComponents.back();
    }

    /**
     * Removes a component. Removal uses "swap-and-pop" - the final dense
     * component is moved into the removed component's slot. Therefore
     * component iteration order is NOT stable.
     * 
     * @param entity The entity whose component should be removed.
     */
    void removeComponent(Entity entity)
    {
        if (!hasComponent(entity)) {
            Console::get().error(
                "[ComponentSparseSet::removeComponent] Entity ID '" +
                std::to_string(entity) +
                "' does not have this component."
            );
        }

        removeExistingComponent(entity);
    }

    /**
     * Returns whether the entity has this component.
     * 
     * @param entity The entity to check for a component.
     * @return True if the entity has this component, false otherwise.
     */
    [[nodiscard]]
    bool hasComponent(Entity entity) const noexcept
    {
        if (entity >= entitiesToDenseIndices.size()) {
            return false;
        }

        const DenseIndex denseIndex = entitiesToDenseIndices[entity];

        return denseIndex != INVALID_INDEX
            && denseIndex < denseEntities.size()
            && denseEntities[denseIndex] == entity;
    }

    /**
     * Returns the entity's component.
     * 
     * @param entity The entity for which to retrieve the component.
     * @return A reference to the component.
     */
    T& getComponent(Entity entity)
    {
        if (!hasComponent(entity)) {
            Console::get().error(
                "[ComponentSparseSet::getComponent] Entity ID '" +
                std::to_string(entity) +
                "' does not have this component."
            );
        }

        return denseComponents[entitiesToDenseIndices[entity]];
    }

    /**
     * Returns the entity's component.
     * 
     * @param entity The entity for which to retrieve the component.
     * @return A reference to the component.
     */
    const T& getComponent(Entity entity) const
    {
        if (!hasComponent(entity)) {
            Console::get().error(
                "[ComponentSparseSet::getComponent] Entity ID '" +
                std::to_string(entity) +
                "' does not have this component."
            );
        }

        return denseComponents[entitiesToDenseIndices[entity]];
    }

    /**
     * Returns the number of components in the dense set.
     * 
     * @return The number of components in the dense set.
     */
    [[nodiscard]]
    std::size_t size() const noexcept override
    {
        return denseComponents.size();
    }


    /**
     * Checks whether the dense set is empty.
     * 
     * @return True if the dense set is empty, false otherwise.
     */
    [[nodiscard]]
    bool empty() const noexcept
    {
        return denseComponents.empty();
    }

    /**
     * Gets the dense array of entities that have this component.
     * 
     * @return The dense array of entities that have this component.
     */
    [[nodiscard]]
    const std::vector<Entity>& getEntities() const noexcept override
    {
        return denseEntities;
    }

    /**
     * Gets the dense array of components.
     * 
     * @return The dense array of components.
     */
    [[nodiscard]]
    std::vector<T>& getComponents() noexcept
    {
        return denseComponents;
    }

    /**
     * Gets the dense array of components.
     * 
     * @return The dense array of components.
     */
    [[nodiscard]]
    const std::vector<T>& getComponents() const noexcept
    {
        return denseComponents;
    }

private:

    /**
     * Removes an entity from the sparse set, using a "swap-and-pop"
     * strategy to maintain dense storage.
     * 
     * @param entity The entity to remove.
     */
    void removeExistingComponent(Entity entity)
    {
        const DenseIndex indexToRemove = entitiesToDenseIndices[entity];
        const DenseIndex lastIndex = static_cast<DenseIndex>(denseComponents.size() - 1);

        if (indexToRemove != lastIndex) {
            // Move the final dense item into the newly-created hole, i.e. "swap-and-pop"!
            denseComponents[indexToRemove] = std::move(denseComponents[lastIndex]);
            denseEntities[indexToRemove] = denseEntities[lastIndex];
            const Entity movedEntity = denseEntities[indexToRemove];
            entitiesToDenseIndices[movedEntity] = indexToRemove;
        }

        denseComponents.pop_back();
        denseEntities.pop_back();

        entitiesToDenseIndices[entity] = INVALID_INDEX;
    }

    std::vector<DenseIndex> entitiesToDenseIndices; // entity ID -> dense index
    std::vector<T> denseComponents; // dense index -> component
    std::vector<Entity> denseEntities; // dense index -> entity
};

/**
 * Metadata associated with an entity slot.
 *
 * The vector index is also the entity ID, but storing entity here makes
 * iteration/debugging convenient.
 */
struct EntityContainer
{
    Entity entity = 0;
    ComponentMask mask{};
    bool isAlive = false;
};

/**
 * Owns entities and all component sparse sets.
 */
class ECS
{
public:
    ECS() = default;
    ~ECS() = default;

    ECS(const ECS&) = delete;
    ECS& operator=(const ECS&) = delete;

    ECS(ECS&&) noexcept = default;
    ECS& operator=(ECS&&) noexcept = default;

    /**
     * Creates an entity.
     *
     * Reclaimed entity IDs are reused.
     */
    Entity createEntity();

    /**
     * Destroys an entity and all of its components.
     * 
     * @param entity The entity to destroy.
     */
    void destroyEntity(Entity entity);

    /**
     * Reports whether an entity currently exists.
     * 
     * @param entity The entity to check for existence.
     * @return True if the entity exists, false otherwise.
     */
    [[nodiscard]]
    bool isAlive(Entity entity) const noexcept;

    /**
     * Number of currently living entities.
     * 
     * @return The number of currently living entities.
     */
    [[nodiscard]]
    std::size_t getEntityCount() const noexcept
    {
        return entityCount;
    }

    /**
     * Entity slot information.
     * 
     * @return The entity slots.
     */
    [[nodiscard]]
    const std::vector<EntityContainer>& getEntities() const noexcept
    {
        return entities;
    }

    /**
     * Gets the numeric ID associated with component T. Will register
     * the component type if it has not been registered yet.
     * 
     * @param T The component type.
     * @return The numeric ID associated with component T.
     */
    template<class T>
    ComponentType getComponentID()
    {
        const std::type_index typeIndex = typeid(T);

        const auto existing = typeToID.find(typeIndex);

        if (existing != typeToID.end()) {
            return existing->second;
        }

        if (typeToID.size() >= MAX_COMPONENTS) {
            Console::get().error(
                "[ECS::getComponentID] Cannot register more than " +
                std::to_string(MAX_COMPONENTS) +
                " component types."
            );
        }

        const ComponentType componentID = typeToID.size();

        Console::get().logOnDebug(
            "[ECS::getComponentID] Registering component type '" +
            std::string(typeIndex.name()) +
            "' as component ID '" +
            std::to_string(componentID) +
            "'."
        );

        typeToID.emplace(typeIndex, componentID);
        components.emplace(typeIndex, std::make_unique<ComponentSparseSet<T>>());

        return componentID;
    }

    /**
     * Adds a default-constructed component.
     * 
     * @param entity The entity to add the component to.
     * @return A reference to the added component.
     */
    template<class T>
    T& addComponent(Entity entity)
    {
        return emplaceComponent<T>(entity);
    }

    /**
     * Adds a component by copying.
     * 
     * @param entity The entity to add the component to.
     * @param component The component to copy.
     * @return A reference to the added component.
     */
    template<class T>
    T& addComponent(Entity entity, const T& component)
    {
        return emplaceComponent<T>(entity, component);
    }

    /**
     * Adds a component by moving.
     * 
     * @param entity The entity to add the component to.
     * @param component The component to move.
     * @return A reference to the added component.
     */
    template<class T>
    T& addComponent(Entity entity, T&& component)
    {
        return emplaceComponent<T>(entity, std::move(component));
    }

    /**
     * Constructs a component directly in its sparse set.
     * 
     * @param entity The entity to add the component to.
     * @param args The arguments to forward to the component's constructor.
     * @return A reference to the added component.
     */
    template<class T, class... Args>
    T& emplaceComponent(Entity entity, Args&&... args)
    {
        validateEntity(entity);

        const ComponentType componentID = getComponentID<T>();

        ComponentSparseSet<T>* componentSparseSet = getComponentSparseSet<T>();

        T& component = componentSparseSet->emplaceComponent(entity, std::forward<Args>(args)...);

        // Only update the mask after component successfully added
        entities[entity].mask.set(componentID);

        return component;
    }

    /**
     * Removes a component from a given entity.
     * 
     * @param entity The entity to remove the component from.
     * @tparam T The type of the component to remove.
     */
    template<class T>
    void removeComponent(Entity entity)
    {
        validateEntity(entity);

        if (!hasComponentID<T>()) {
            Console::get().error(
                "[ECS::removeComponent] Component type '" +
                std::string(typeid(T).name()) +
                "' has not been registered."
            );
        }

        const ComponentType& componentID = getComponentID<T>();
        ComponentSparseSet<T>* componentSparseSet = getComponentSparseSet<T>();

        if (!componentSparseSet) {
            Console::get().error(
                "[ECS::removeComponent] ComponentSparseSet '" +
                std::string(typeid(T).name()) +
                "' returned a null pointer."
            );
            return;
        }

        componentSparseSet->removeComponent(entity);
        entities[entity].mask.reset(componentID); // Only update the mask after component has been successfully removed
    }

    /**
     * Checks whether a entity has a component of type T.
     * 
     * @param entity The entity to check for a component.
     * @tparam T The type of the component to check for.
     * @return True if the entity has a component of type T, false otherwise.
     */
    template<class T>
    [[nodiscard]]
    bool hasComponent(Entity entity) const
    {
        validateEntity(entity);

        const std::type_index typeIndex = typeid(T);
        const auto component = components.find(typeIndex);

        if (component == components.end()) {
            return false;
        }

        return true;
    }

    /**
     * Gets a component of type T from a given entity.
     * 
     * @param entity The entity to get the component from.
     * @tparam T The type of the component to get.
     * @return A reference to the component.
     */
    template<class T>
    T& getComponent(Entity entity)
    {
        validateEntity(entity);
        return getComponentSparseSet<T>()->getComponent(entity);
    }

    /**
     * Gets a component of type T from a given entity.
     * 
     * @param entity The entity to get the component from.
     * @tparam T The type of the component to get.
     * @return A reference to the component.
     */
    template<class T>
    const T& getComponent(Entity entity) const
    {
        validateEntity(entity);
        return getComponentSparseSet<T>()->getComponent(entity);
    }

    /**
     * Collects all entities with a given set of components.
     * 
     * @tparam Components The component types to query for.
     * @return A vector of entities that have all the specified components.
     */
    template<class... Components>
    [[nodiscard]]
    std::vector<Entity> query() const
    {
        if (sizeof...(Components) == 0) {
            Console::get().error("[ECS::query] At least one component type must be specified.");
        }

        ComponentMask requiredMask;
        const IComponentSparseSet* smallestSparseSet = buildQuery<Components...>(requiredMask);

        if (smallestSparseSet == nullptr) {
            return {};
        }

        std::vector<Entity> result;
        result.reserve(smallestSparseSet->size());

        for (Entity entity : smallestSparseSet->getEntities()) {
            const EntityContainer& entityContainer = entities[entity];

            if (!entityContainer.isAlive) {
                continue;
            }

            if ((entityContainer.mask & requiredMask) != requiredMask) {
                continue;
            }

            result.push_back(entity);
        }

        return result;
    }

    /**
     * Invokes `func(entity, Components&...)` for all matching entities.
     * 
     * @tparam Components The component types to query for.
     * @tparam Func The type of the function to invoke.
     * @param func The function to invoke.
     * @warning Adding/removing components from inside `func` can invalidate
     * iteration if it mutates the sparse set we're currently traversing.
     * Structural changes should normally be deferred until after iteration.
     * @note This is a non-const version of forEach. Use the const version
     * if you want to ensure that `func` does not modify the ECS.
     */
    template<class... Components, class Func>
    void forEach(Func&& func)
    {
        if (sizeof...(Components) == 0) {
            Console::get().error("[ECS::forEach] At least one component type must be specified.");
        }

        ComponentMask requiredMask;
        const IComponentSparseSet* smallestSparseSet = buildQuery<Components...>(requiredMask);

        if (smallestSparseSet == nullptr) {
            return;
        }

        const auto& matchingCandidates = smallestSparseSet->getEntities();

        for (const Entity entity : matchingCandidates) {
            if (!entities[entity].isAlive) {
                continue;
            }

            if ((entities[entity].mask & requiredMask) != requiredMask) {
                continue;
            }

            std::invoke(func, entity, getComponentSparseSet<Components>()->getComponent(entity)...);
        }
    }

    /**
     * Invokes `func(entity, Components&...)` for all matching entities.
     * 
     * @tparam Components The component types to query for.
     * @tparam Func The type of the function to invoke.
     * @param func The function to invoke.
     * @warning Adding/removing components from inside `func` can invalidate
     * iteration if it mutates the sparse set we're currently traversing.
     * Structural changes should normally be deferred until after iteration.
     * @note This is a const version of forEach. Use the non-const version
     * if you want to allow `func` to modify the ECS.
     */
    template<class... Components, class Func>
    void forEach(Func&& func) const
    {
        if (sizeof...(Components) == 0) {
            Console::get().error("[ECS::forEach] At least one component type must be specified.");
        }

        ComponentMask requiredMask;
        const IComponentSparseSet* smallestSparseSet = buildQuery<Components...>(requiredMask);

        if (smallestSparseSet == nullptr) {
            return;
        }

        for (Entity entity : smallestSparseSet->getEntities()) {
            if (!entities[entity].isAlive) {
                continue;
            }

            if ((entities[entity].mask & requiredMask) != requiredMask) {
                continue;
            }

            std::invoke(func, entity, getComponentSparseSet<Components>()->getComponent(entity)...);
        }
    }

private:
    std::vector<EntityContainer> entities;
    std::vector<Entity> availableEntities;
    std::unordered_map<std::type_index, std::unique_ptr<IComponentSparseSet>> components; // Map of component types to their corresponding sparse sets
    std::unordered_map<std::type_index, ComponentType> typeToID;
    std::size_t entityCount = 0;

    
    void validateEntity(Entity entity) const;

    /**
     * Checks whether a component type has been registered.
     * 
     * @tparam T The component type to check.
     * @return True if the component type has been registered, false otherwise.
     */
    template<class T>
    [[nodiscard]]
    bool hasComponentID() const
    {
        const auto component = typeToID.find(std::type_index(typeid(T)));

        if (component == typeToID.end()) {
            return false;
        }

        return true;
    }

    /**
     * Gets a registered component sparse set.
     * 
     * @tparam T The component type to get the sparse set for.
     * @return A raw pointer to the component sparse set.
     */
    template<class T>
    ComponentSparseSet<T>* getComponentSparseSet()
    {
        const std::type_index typeIndex = typeid(T);

        const auto component = components.find(typeIndex);

        if (component == components.end()) {
            Console::get().error(
                "[ECS::getComponentSparseSet] Component type '" +
                std::string(typeIndex.name()) +
                "' is not registered."
            );
        }

        return static_cast<ComponentSparseSet<T>*>(component->second.get());
    }

    template<class T>
    const ComponentSparseSet<T>* getComponentSparseSet() const
    {
        const std::type_index typeIndex = typeid(T);

        const auto component = components.find(typeIndex);

        if (component == components.end()) {
            Console::get().error(
                "[ECS::getComponentSparseSet] Component type '" +
                std::string(typeIndex.name()) +
                "' is not registered."
            );
        }

        return static_cast<const ComponentSparseSet<T>*>(component->second.get());
    }

    /**
     * Builds a query mask and finds the smallest sparse set involved.
     * If any requested component type has never been registered, no entity can
     * possibly match the query and nullptr is returned.
     * 
     * @tparam Components The component types to query for.
     * @param requiredMask The mask to populate with the requested component types.
     * @return A pointer to the smallest sparse set involved in the query, or
     * nullptr if any component type is unregistered.
     */
    template<class... Components>
    const IComponentSparseSet* buildQuery(ComponentMask& requiredMask) const
    {
        const IComponentSparseSet* smallestSet = nullptr;
        bool allRegistered = true;

        auto addComponentToQuery = [this, &requiredMask, &smallestSet, &allRegistered]<class T>() {
            const std::type_index typeIndex = typeid(T);
            const auto id = typeToID.find(typeIndex);
            const auto component = components.find(typeIndex);                

            if (id == typeToID.end() || component == components.end()) {
                allRegistered = false;
                return;
            }

            requiredMask.set(id->second);

            const IComponentSparseSet* set = component->second.get();

            if (smallestSet == nullptr || set->size() < smallestSet->size()) {
                smallestSet = set;
            }
        };

        (addComponentToQuery.template operator()<Components>(), ...);

        if (!allRegistered) {
            return nullptr;
        }

        return smallestSet;
    }
};