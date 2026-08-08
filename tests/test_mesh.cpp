#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <utility>

#include "graphics/material/material.hpp"
#include "graphics/mesh/mesh.hpp"

TEST_CASE("a mesh owns its material")
{
    auto material = std::make_shared<Material>();
    std::weak_ptr<Material> materialObserver = material;
    Mesh mesh;

    mesh.setMaterial(std::move(material));

    REQUIRE_FALSE(materialObserver.expired());
}

TEST_CASE("replacing a mesh material releases the previous material")
{
    auto previousMaterial = std::make_shared<Material>();
    std::weak_ptr<Material> previousMaterialObserver = previousMaterial;
    Mesh mesh;
    mesh.setMaterial(std::move(previousMaterial));

    mesh.setMaterial(std::make_shared<Material>());

    REQUIRE(previousMaterialObserver.expired());
}

TEST_CASE("a shared material survives until its final mesh is destroyed")
{
    auto material = std::make_shared<Material>();
    std::weak_ptr<Material> materialObserver = material;
    auto firstMesh = std::make_unique<Mesh>();
    auto secondMesh = std::make_unique<Mesh>();
    firstMesh->setMaterial(material);
    secondMesh->setMaterial(material);
    material.reset();

    firstMesh.reset();
    REQUIRE_FALSE(materialObserver.expired());

    secondMesh.reset();
    REQUIRE(materialObserver.expired());
}
