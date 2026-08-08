#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <stdexcept>
#include <utility>

#include "core/ecs/component_manager.hpp"
#include "core/ecs/entity_manager.hpp"

namespace
{
    struct Position
    {
        int x = 0;
    };

    struct Velocity
    {
        int x = 0;
    };

    template<std::size_t Index>
    struct IndexedComponent
    {
    };

    template<std::size_t... Indices>
    void registerComponents(ComponentManager& components, std::index_sequence<Indices...>)
    {
        (components.getComponentID<IndexedComponent<Indices>>(), ...);
    }

    struct ECSManagers
    {
        std::shared_ptr<ComponentManager> components = std::make_shared<ComponentManager>();
        EntityManager entities{components};
    };
}

TEST_CASE("entities retain stable IDs and reuse destroyed IDs", "[ecs][entity]")
{
    ECSManagers ecs;
    const Entity first = ecs.entities.createEntity();
    const Entity second = ecs.entities.createEntity();
    const Entity third = ecs.entities.createEntity();

    ecs.entities.addComponent<Position>(third, {42});
    ecs.entities.destroyEntity(second);

    REQUIRE(ecs.entities.isAlive(first));
    REQUIRE_FALSE(ecs.entities.isAlive(second));
    REQUIRE(ecs.entities.isAlive(third));
    REQUIRE(ecs.entities.getComponent<Position>(third).x == 42);

    const Entity reused = ecs.entities.createEntity();
    REQUIRE(reused == second);
    REQUIRE(ecs.entities.isAlive(reused));
    REQUIRE_FALSE(ecs.entities.hasComponent<Position>(reused));
    REQUIRE(ecs.entities.getEntityCount() == 3);
}

TEST_CASE("components can be added, replaced, queried, and removed", "[ecs][component]")
{
    ECSManagers ecs;
    const Entity entity = ecs.entities.createEntity();

    ecs.entities.addComponent<Position>(entity, {10});
    REQUIRE(ecs.entities.hasComponent<Position>(entity));
    REQUIRE(ecs.entities.getComponent<Position>(entity).x == 10);

    ecs.entities.addComponent<Position>(entity, {20});
    REQUIRE(ecs.entities.getComponent<Position>(entity).x == 20);

    ecs.entities.removeComponent<Position>(entity);
    REQUIRE_FALSE(ecs.entities.hasComponent<Position>(entity));
    REQUIRE_THROWS_AS(ecs.entities.getComponent<Position>(entity), std::runtime_error);
    REQUIRE_THROWS_AS(ecs.entities.removeComponent<Position>(entity), std::runtime_error);
}

TEST_CASE("destroying an entity removes all of its components", "[ecs][entity][component]")
{
    ECSManagers ecs;
    const Entity entity = ecs.entities.createEntity();
    ecs.entities.addComponent<Position>(entity, {5});
    ecs.entities.addComponent<Velocity>(entity, {7});

    ecs.entities.destroyEntity(entity);

    REQUIRE_FALSE(ecs.components->hasComponent<Position>(entity));
    REQUIRE_FALSE(ecs.components->hasComponent<Velocity>(entity));
    REQUIRE_THROWS_AS(ecs.entities.destroyEntity(entity), std::runtime_error);
    REQUIRE_THROWS_AS(ecs.entities.addComponent<Position>(entity), std::runtime_error);
}

TEST_CASE("entity and component limits are enforced", "[ecs][limits]")
{
    ECSManagers ecs;
    for (Entity entity = 0; entity < MAX_ENTITIES; entity++) {
        REQUIRE(ecs.entities.createEntity() == entity);
    }
    REQUIRE_THROWS_AS(ecs.entities.createEntity(), std::runtime_error);

    ComponentManager components;
    registerComponents(components, std::make_index_sequence<MAX_COMPONENTS>{});
    REQUIRE_THROWS_AS(
        components.getComponentID<IndexedComponent<MAX_COMPONENTS>>(),
        std::runtime_error
    );
}
