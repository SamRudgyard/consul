#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <utility>

#include "graphics/material/material.hpp"
#include "graphics/mesh/mesh.hpp"
#include "graphics/models/model.hpp"

TEST_CASE("a model owns its primitive assets")
{
    auto mesh = std::make_shared<Mesh>();
    auto material = std::make_shared<Material>();
    std::weak_ptr<Mesh> meshObserver = mesh;
    std::weak_ptr<Material> materialObserver = material;
    Model model;

    model.addPrimitive(std::move(mesh), std::move(material));

    REQUIRE_FALSE(meshObserver.expired());
    REQUIRE_FALSE(materialObserver.expired());
    REQUIRE(model.getPrimitives().size() == 1);
}

TEST_CASE("shared primitive assets survive until their final model is destroyed")
{
    auto mesh = std::make_shared<Mesh>();
    auto material = std::make_shared<Material>();
    std::weak_ptr<Mesh> meshObserver = mesh;
    std::weak_ptr<Material> materialObserver = material;
    auto firstModel = std::make_unique<Model>();
    auto secondModel = std::make_unique<Model>();
    firstModel->addPrimitive(mesh, material);
    secondModel->addPrimitive(mesh, material);
    mesh.reset();
    material.reset();

    firstModel.reset();
    REQUIRE_FALSE(meshObserver.expired());
    REQUIRE_FALSE(materialObserver.expired());

    secondModel.reset();
    REQUIRE(meshObserver.expired());
    REQUIRE(materialObserver.expired());
}

TEST_CASE("a model primitive preserves its local transform")
{
    glm::mat4 localTransform(1.0f);
    localTransform[3][0] = 2.0f;
    localTransform[3][1] = 3.0f;

    Model model;
    model.addPrimitive(std::make_shared<Mesh>(), std::make_shared<Material>(), localTransform);

    REQUIRE(model.getPrimitives().front().localTransform[3][0] == 2.0f);
    REQUIRE(model.getPrimitives().front().localTransform[3][1] == 3.0f);
}
