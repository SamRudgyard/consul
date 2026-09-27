#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <utility>

#include "graphics/material/material.hpp"
#include "graphics/mesh/mesh.hpp"
#include "graphics/models/model.hpp"

TEST_CASE("A model owns its mesh, material, and local transform")
{
    auto mesh = std::make_shared<Mesh>();
    auto material = std::make_shared<Material>();
    std::weak_ptr<Mesh> meshObserver = mesh;
    std::weak_ptr<Material> materialObserver = material;
    Model model;

    model.addMesh(mesh, material);

    REQUIRE_FALSE(meshObserver.expired());
    REQUIRE_FALSE(materialObserver.expired());
    REQUIRE(model.getMeshes().size() == 1);
    REQUIRE(model.getMaterials().size() == 1);
    REQUIRE(model.getLocalTransforms().size() == 1);
}

TEST_CASE("A model assets survive until their final reference is destroyed")
{
    auto mesh = std::make_shared<Mesh>();
    auto material = std::make_shared<Material>();
    std::weak_ptr<Mesh> meshObserver = mesh;
    std::weak_ptr<Material> materialObserver = material;
    auto firstModel = std::make_unique<Model>();
    auto secondModel = std::make_unique<Model>();
    firstModel->addMesh(mesh, material);
    secondModel->addMesh(mesh, material);
    mesh.reset();
    material.reset();

    firstModel.reset();
    REQUIRE_FALSE(meshObserver.expired());
    REQUIRE_FALSE(materialObserver.expired());

    secondModel.reset();
    REQUIRE(meshObserver.expired());
    REQUIRE(materialObserver.expired());
}

TEST_CASE("A model correctly stores its local transform")
{
    glm::mat4 localTransform(1.0f);
    localTransform[3][0] = 2.0f;
    localTransform[3][1] = 3.0f;

    Model model;
    model.addMesh(std::make_shared<Mesh>(), std::make_shared<Material>(), localTransform);

    REQUIRE(model.getLocalTransforms().front()[3][0] == 2.0f);
    REQUIRE(model.getLocalTransforms().front()[3][1] == 3.0f);
}
