#pragma once

#include "glm/glm.hpp"

using namespace glm;


struct Physics2D {
    float oneMass = 0.f; // Analogous to mass - determines the acceleration of the object, where greater oneMass results in greater acceleration
    vec2 velocity = vec2(0.f, 0.f);
    vec2 acceleration = vec2(0.f, 0.f);
    float coefficientOfFriction = 0.f;
};