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

    Mesh cube(float width);
    Mesh sphereUV(float radius, unsigned int nLatitudes, unsigned int nLongitudes);
    Mesh sphereIcosphere(float radius, unsigned int nDivisions);

private:
    Geometry() {}
    ~Geometry() {}
};
