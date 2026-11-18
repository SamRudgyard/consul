#include <catch2/catch_test_macros.hpp>

#include <vector>

#include "graphics/mesh/mesh.hpp"

TEST_CASE("a mesh stores its geometry")
{
    const std::vector<glm::vec3> positions = {
        {0.0f, 0.0f, 0.0f},
        {1.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f}
    };
    const std::vector<unsigned int> indices = {0, 1, 2};

    Mesh mesh(positions, indices);

    REQUIRE(mesh.getPositions() == positions);
    REQUIRE(mesh.getIndices() == indices);
    REQUIRE(mesh.getNumIndices() == indices.size());
}

TEST_CASE("line draw mode expands triangle indices into edges")
{
    const std::vector<glm::vec3> positions = {
        {0.0f, 0.0f, 0.0f},
        {1.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f}
    };
    Mesh mesh(positions, {0, 1, 2}, DrawMode::LINES);

    REQUIRE(mesh.getIndices() == std::vector<unsigned int>{0, 1, 1, 2, 2, 0});
    REQUIRE(mesh.getNumIndices() == 6);
}
