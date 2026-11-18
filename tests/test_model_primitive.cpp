#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <utility>

#include "graphics/material/material.hpp"
#include "graphics/mesh/mesh.hpp"
#include "graphics/model_primitive/model_primitive.hpp"

TEST_CASE("A model primitive owns its mesh, material, and local transform")
{
    auto mesh = std::make_shared<Mesh>();
    auto material = std::make_shared<Material>();
    std::weak_ptr<Material> materialObserver = material;
    ModelPrimitive primitive(mesh, material);

    REQUIRE(primitive.getMesh() == mesh);
    REQUIRE(primitive.getMaterial() == material);
}

TEST_CASE("Assets within a model primitive survive until their final reference is destroyed")
{
    auto mesh = std::make_shared<Mesh>();
    auto material = std::make_shared<Material>();
    std::weak_ptr<Mesh> meshObserver = mesh;
    std::weak_ptr<Material> materialObserver = material;
    auto firstModel = std::make_unique<ModelPrimitive>(mesh, material);
    auto secondModel = std::make_unique<ModelPrimitive>(mesh, material);
    mesh.reset();
    material.reset();

    firstModel.reset();
    REQUIRE_FALSE(meshObserver.expired());
    REQUIRE_FALSE(materialObserver.expired());

    secondModel.reset();
    REQUIRE(meshObserver.expired());
    REQUIRE(materialObserver.expired());
}

TEST_CASE("A model primitive correctly stores its local transform")
{
    glm::mat4 localTransform(1.0f);
    localTransform[3][0] = 2.0f;
    localTransform[3][1] = 3.0f;

    ModelPrimitive primitive(std::make_shared<Mesh>(), std::make_shared<Material>(), localTransform);
    REQUIRE(primitive.getLocalTransform()[3][0] == localTransform[3][0]);
    REQUIRE(primitive.getLocalTransform()[3][1] == localTransform[3][1]);
}
