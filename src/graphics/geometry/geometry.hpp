#pragma once

#include "graphics/mesh/mesh.hpp"
#include "graphics/colour.hpp"

#include "glm/vec4.hpp"

class Geometry
{
public:
    static Geometry* get()
    {
        static Geometry instance;
        return &instance;
    }

    Mesh capsule(float radius, float height, unsigned int nLatitudes, unsigned int nLongitudes);
    Mesh cone(float radius, float height, unsigned int sides);
    Mesh cylinder(float radiusTop, float radiusBottom, float height, unsigned int sides);
    Mesh cube(float width);
    Mesh cuboid(float width, float height, float depth);
    Mesh pyramidSquare(float baseSize, float height);
    Mesh sphereUV(float radius, unsigned int nLatitudes, unsigned int nLongitudes);
    Mesh sphereIcosphere(float radius, unsigned int nDivisions);

private:
    Geometry() {}
    ~Geometry() {}
};
