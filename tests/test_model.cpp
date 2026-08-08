#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <utility>

#include "graphics/mesh/mesh.hpp"
#include "graphics/models/model.hpp"

TEST_CASE("a model owns its meshes")
{
    auto mesh = std::make_shared<Mesh>();
    std::weak_ptr<Mesh> meshObserver = mesh;
    Model model;

    model.addMesh(std::move(mesh));

    REQUIRE_FALSE(meshObserver.expired());
    REQUIRE(model.getMeshes().size() == 1);
}

TEST_CASE("a shared mesh survives until its final model is destroyed")
{
    auto mesh = std::make_shared<Mesh>();
    std::weak_ptr<Mesh> meshObserver = mesh;
    auto firstModel = std::make_unique<Model>();
    auto secondModel = std::make_unique<Model>();
    firstModel->addMesh(mesh);
    secondModel->addMesh(mesh);
    mesh.reset();

    firstModel.reset();
    REQUIRE_FALSE(meshObserver.expired());

    secondModel.reset();
    REQUIRE(meshObserver.expired());
}
