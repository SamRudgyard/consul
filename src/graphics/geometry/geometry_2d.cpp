#include "graphics/geometry/geometry_2d.hpp"

Mesh Geometry2D::line(glm::vec2 startPosition, glm::vec2 endPosition)
{
    std::vector<glm::vec3> positions = {
        {startPosition.x, startPosition.y, 0.0f},
        {endPosition.x, endPosition.y, 0.0f}
    };
    std::vector<unsigned int> indices = {0, 1};
    std::vector<Texture> textures;
    textures.emplace_back(Texture());

    return Mesh(std::move(positions), std::move(indices), std::move(textures), Colour(255, 255, 255, 255), DrawMode::LINES);
}

Mesh Geometry2D::rect(glm::vec2 minCorner, glm::vec2 maxCorner)
{
    std::vector<glm::vec3> positions = {
        {minCorner.x, minCorner.y, 0.0f},
        {maxCorner.x, minCorner.y, 0.0f},
        {minCorner.x, maxCorner.y, 0.0f},
        {maxCorner.x, maxCorner.y, 0.0f}
    };
    std::vector<unsigned int> indices = {
        0, 1, 2,
        2, 1, 3
    };
    std::vector<Texture> textures;
    textures.emplace_back(Texture());

    return Mesh(std::move(positions), std::move(indices), std::move(textures), Colour(255, 255, 255, 255), DrawMode::TRIANGLES);
}

Mesh Geometry2D::rectOutline(glm::vec2 minCorner, glm::vec2 maxCorner)
{
    std::vector<glm::vec3> positions = {
        {minCorner.x, minCorner.y, 0.0f},
        {maxCorner.x, minCorner.y, 0.0f},
        {maxCorner.x, maxCorner.y, 0.0f},
        {minCorner.x, maxCorner.y, 0.0f}
    };
    std::vector<unsigned int> indices = {
        0, 1,
        1, 2,
        2, 3,
        3, 0
    };
    std::vector<Texture> textures;
    textures.emplace_back(Texture());

    return Mesh(std::move(positions), std::move(indices), std::move(textures), Colour(255, 255, 255, 255), DrawMode::LINES);
}

Mesh Geometry2D::triangle(glm::vec2 a, glm::vec2 b, glm::vec2 c)
{
    std::vector<glm::vec3> positions = {
        {a.x, a.y, 0.0f},
        {b.x, b.y, 0.0f},
        {c.x, c.y, 0.0f}
    };
    std::vector<unsigned int> indices = {0, 1, 2};
    std::vector<Texture> textures;
    textures.emplace_back(Texture());

    return Mesh(std::move(positions), std::move(indices), std::move(textures), Colour(255, 255, 255, 255), DrawMode::TRIANGLES);
}
