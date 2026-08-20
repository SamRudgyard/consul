#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

#include "core/ecs/components.hpp"
#include "core/ecs/ecs.hpp"
#include "graphics/models/model.hpp"

namespace
{
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

    Transform transform;
    transform.position.x = 42.0f;
    ecs.addComponent<Transform>(third, transform);
    ecs.destroyEntity(second);

    REQUIRE(ecs.isAlive(first));
    REQUIRE_FALSE(ecs.isAlive(second));
    REQUIRE(ecs.isAlive(third));
    REQUIRE(ecs.getComponent<Transform>(third).position.x == 42.0f);

    const Entity reused = ecs.createEntity();
    REQUIRE(reused == second);
    REQUIRE(ecs.isAlive(reused));
    REQUIRE_FALSE(ecs.hasComponent<Transform>(reused));
    REQUIRE(ecs.getEntityCount() == 3);
}

TEST_CASE("components can be added, queried, and removed", "[ecs][component]")
{
    ECS ecs;
    const Entity entity = ecs.createEntity();

    Transform transform;
    transform.position.x = 10.0f;
    ecs.addComponent<Transform>(entity, transform);
    REQUIRE(ecs.hasComponent<Transform>(entity));
    REQUIRE(ecs.getComponent<Transform>(entity).position.x == 10.0f);

    transform.position.x = 20.0f;
    REQUIRE_THROWS_AS(ecs.addComponent<Transform>(entity, transform), std::runtime_error);
    REQUIRE(ecs.getComponent<Transform>(entity).position.x == 10.0f);

    ecs.removeComponent<Transform>(entity);
    REQUIRE_FALSE(ecs.hasComponent<Transform>(entity));
    REQUIRE_THROWS_AS(ecs.getComponent<Transform>(entity), std::runtime_error);
    REQUIRE_THROWS_AS(ecs.removeComponent<Transform>(entity), std::runtime_error);
}

TEST_CASE("destroying an entity removes all of its components", "[ecs][entity][component]")
{
    ECS ecs;
    const Entity entity = ecs.createEntity();
    ecs.addComponent<Transform>(entity);
    ecs.addComponent<Velocity>(entity, {7.0f});

    ecs.destroyEntity(entity);

    REQUIRE(ecs.query<Transform>().empty());
    REQUIRE(ecs.query<Velocity>().empty());
    REQUIRE_THROWS_AS(ecs.destroyEntity(entity), std::runtime_error);
    REQUIRE_THROWS_AS(ecs.addComponent<Transform>(entity), std::runtime_error);
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

    ecs.addComponent<Transform>(positionOnly);
    ecs.addComponent<Transform>(both);
    ecs.addComponent<Velocity>(both, {20.0f});
    ecs.addComponent<Transform>(destroyed);
    ecs.addComponent<Velocity>(destroyed, {30.0f});
    ecs.addComponent<Velocity>(velocityOnly, {40.0f});
    ecs.destroyEntity(destroyed);

    REQUIRE(ecs.query<Transform>() == std::vector<Entity>{positionOnly, both});
    REQUIRE(ecs.query<Velocity>() == std::vector<Entity>{both, velocityOnly});
    REQUIRE(ecs.query<Transform, Velocity>() == std::vector<Entity>{both});

    REQUIRE(ecs.query<Camera>().empty());

    ecs.removeComponent<Velocity>(both);
    REQUIRE(ecs.query<Transform, Velocity>().empty());
}

TEST_CASE("forEach exposes entity IDs and component references", "[ecs][query]")
{
    ECS ecs;
    const Entity first = ecs.createEntity();
    const Entity second = ecs.createEntity();
    const Entity ignored = ecs.createEntity();

    Transform firstTransform;
    firstTransform.position.x = 1.0f;
    ecs.addComponent<Transform>(first, firstTransform);

    Transform secondTransform;
    secondTransform.position.x = 2.0f;
    ecs.addComponent<Transform>(second, secondTransform);

    ecs.addComponent<Velocity>(ignored, {3.0f});

    std::vector<Entity> visited;
    ecs.forEach<Transform>([&](Entity entity, Transform& transform) {
        visited.push_back(entity);
        transform.position.x *= 10.0f;
    });

    REQUIRE(visited == std::vector<Entity>{first, second});
    REQUIRE(ecs.getComponent<Transform>(first).position.x == 10.0f);
    REQUIRE(ecs.getComponent<Transform>(second).position.x == 20.0f);

    float total = 0.0f;
    ecs.forEach<Transform>([&](Entity entity, Transform& transform) {
        total += static_cast<float>(entity) + transform.position.x;
    });
    REQUIRE(total == static_cast<float>(first + second) + 30.0f);
}

TEST_CASE("render components can be queried and release their model assets", "[ecs][component][render]")
{
    ECS ecs;
    const Entity entity = ecs.createEntity();

    Transform transform;
    transform.position = {1.0f, 2.0f, 3.0f};

    std::shared_ptr<Model> model = std::make_shared<Model>();
    std::weak_ptr<Model> modelObserver = model;

    ecs.addComponent<Transform>(entity, transform);
    ecs.addComponent<ModelRenderer>(entity, ModelRenderer{model, true});
    model.reset();

    bool visited = false;
    ecs.forEach<Transform, ModelRenderer>(
        [&](Entity visitedEntity, Transform& storedTransform, ModelRenderer& renderer) {
            visited = true;
            REQUIRE(visitedEntity == entity);
            REQUIRE(storedTransform.position == glm::vec3(1.0f, 2.0f, 3.0f));
            REQUIRE(renderer.model != nullptr);
            REQUIRE(renderer.visible);
        }
    );

    REQUIRE(visited);
    REQUIRE_FALSE(modelObserver.expired());

    ecs.destroyEntity(entity);

    REQUIRE(modelObserver.expired());
}
