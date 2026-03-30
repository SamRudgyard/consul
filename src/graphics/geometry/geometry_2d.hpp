#pragma once

#include "graphics/mesh/mesh.hpp"
#include "graphics/colour.hpp"
#include "glm/glm.hpp"

class Geometry2D
{
public:
    static Geometry2D* get()
    {
        static Geometry2D instance;
        return &instance;
    }

    Mesh line(glm::vec2 startPosition, glm::vec2 endPosition);
    Mesh rect(glm::vec2 minCorner, glm::vec2 maxCorner);
    Mesh rectOutline(glm::vec2 minCorner, glm::vec2 maxCorner);
    Mesh triangle(glm::vec2 a, glm::vec2 b, glm::vec2 c);

private:
    Geometry2D() {}
    ~Geometry2D() {}
};
