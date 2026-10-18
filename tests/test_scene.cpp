#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include <memory>

#include "core/ecs/components.hpp"
#include "core/engine.hpp"
#include "core/project/scene.hpp"
#include "core/project/scene_manager.hpp"
#include "graphics/material/material.hpp"
#include "graphics/mesh/mesh.hpp"
#include "graphics/models/model.hpp"
#include "graphics/renderer/renderer.hpp"
#include "graphics/shader/shader.hpp"

TEST_CASE("destroying an uninitialised scene is safe")
{
    Scene scene;
}

TEST_CASE("scenes own independent ECS worlds")
{
    Scene firstScene;
    Scene secondScene;

    const Entity entity = firstScene.getECS().createEntity();

    REQUIRE(firstScene.getECS().isAlive(entity));
    REQUIRE(firstScene.getECS().getEntityCount() == 1);
    REQUIRE(secondScene.getECS().getEntityCount() == 0);

    const Scene& constScene = firstScene;
    REQUIRE(constScene.getECS().isAlive(entity));
}