#include <catch2/catch_test_macros.hpp>

#include <stdexcept>
#include <utility>
#include <vector>

#include "core/ecs/ecs.hpp"

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

    struct Health
    {
        int value = 0;
    };

    template<std::size_t Index>
    struct IndexedComponent
    {
    };

    template<std::size_t... Indices>
    void registerComponents(ECS& ecs, std::index_sequence<Indices...>)
    {
        (ecs.getComponentID<IndexedComponent<Indices>>(), ...);
    }
}

TEST_CASE("entities retain stable IDs and reuse destroyed IDs", "[ecs][entity]")
{
    ECS ecs;
    const Entity first = ecs.createEntity();
    const Entity second = ecs.createEntity();
    const Entity third = ecs.createEntity();

    ecs.addComponent<Position>(third, {42});
    ecs.destroyEntity(second);

    REQUIRE(ecs.isAlive(first));
    REQUIRE_FALSE(ecs.isAlive(second));
    REQUIRE(ecs.isAlive(third));
    REQUIRE(ecs.getComponent<Position>(third).x == 42);

    const Entity reused = ecs.createEntity();
    REQUIRE(reused == second);
    REQUIRE(ecs.isAlive(reused));
    REQUIRE_FALSE(ecs.hasComponent<Position>(reused));
    REQUIRE(ecs.getEntityCount() == 3);
}

TEST_CASE("components can be added, queried, and removed", "[ecs][component]")
{
    ECS ecs;
    const Entity entity = ecs.createEntity();

    ecs.addComponent<Position>(entity, {10});
    REQUIRE(ecs.hasComponent<Position>(entity));
    REQUIRE(ecs.getComponent<Position>(entity).x == 10);

    REQUIRE_THROWS_AS(ecs.addComponent<Position>(entity, {20}), std::runtime_error);
    REQUIRE(ecs.getComponent<Position>(entity).x == 10);

    ecs.removeComponent<Position>(entity);
    REQUIRE_FALSE(ecs.hasComponent<Position>(entity));
    REQUIRE_THROWS_AS(ecs.getComponent<Position>(entity), std::runtime_error);
    REQUIRE_THROWS_AS(ecs.removeComponent<Position>(entity), std::runtime_error);
}

TEST_CASE("destroying an entity removes all of its components", "[ecs][entity][component]")
{
    ECS ecs;
    const Entity entity = ecs.createEntity();
    ecs.addComponent<Position>(entity, {5});
    ecs.addComponent<Velocity>(entity, {7});

    ecs.destroyEntity(entity);

    REQUIRE(ecs.query<Position>().empty());
    REQUIRE(ecs.query<Velocity>().empty());
    REQUIRE_THROWS_AS(ecs.destroyEntity(entity), std::runtime_error);
    REQUIRE_THROWS_AS(ecs.addComponent<Position>(entity), std::runtime_error);
}

TEST_CASE("entity and component limits are enforced", "[ecs][limits]")
{
    ECS ecs;
    for (Entity entity = 0; entity < MAX_ENTITIES; entity++) {
        REQUIRE(ecs.createEntity() == entity);
    }
    REQUIRE_THROWS_AS(ecs.createEntity(), std::runtime_error);

    ECS componentECS;
    registerComponents(componentECS, std::make_index_sequence<MAX_COMPONENTS>{});
    REQUIRE_THROWS_AS(
        componentECS.getComponentID<IndexedComponent<MAX_COMPONENTS>>(),
        std::runtime_error
    );
}

TEST_CASE("queries match living entities by component signature", "[ecs][query]")
{
    ECS ecs;
    const Entity positionOnly = ecs.createEntity();
    const Entity both = ecs.createEntity();
    const Entity destroyed = ecs.createEntity();
    const Entity velocityOnly = ecs.createEntity();

    ecs.addComponent<Position>(positionOnly, {1});
    ecs.addComponent<Position>(both, {2});
    ecs.addComponent<Velocity>(both, {20});
    ecs.addComponent<Position>(destroyed, {3});
    ecs.addComponent<Velocity>(destroyed, {30});
    ecs.addComponent<Velocity>(velocityOnly, {40});
    ecs.destroyEntity(destroyed);

    REQUIRE(ecs.query<Position>() == std::vector<Entity>{positionOnly, both});
    REQUIRE(ecs.query<Velocity>() == std::vector<Entity>{both, velocityOnly});
    REQUIRE(ecs.query<Position, Velocity>() == std::vector<Entity>{both});

    REQUIRE(ecs.query<Health>().empty());

    ecs.removeComponent<Velocity>(both);
    REQUIRE(ecs.query<Position, Velocity>().empty());
}

TEST_CASE("forEach exposes entity IDs and component references", "[ecs][query]")
{
    ECS ecs;
    const Entity first = ecs.createEntity();
    const Entity second = ecs.createEntity();
    const Entity ignored = ecs.createEntity();

    ecs.addComponent<Position>(first, {1});
    ecs.addComponent<Position>(second, {2});
    ecs.addComponent<Velocity>(ignored, {3});

    std::vector<Entity> visited;
    ecs.forEach<Position>([&](Entity entity, Position& position) {
        visited.push_back(entity);
        position.x *= 10;
    });

    REQUIRE(visited == std::vector<Entity>{first, second});
    REQUIRE(ecs.getComponent<Position>(first).x == 10);
    REQUIRE(ecs.getComponent<Position>(second).x == 20);

    int total = 0;
    ecs.forEach<Position>([&](Entity entity, Position& position) {
        total += static_cast<int>(entity) + position.x;
    });
    REQUIRE(total == static_cast<int>(first + second) + 30);
}
