#pragma once

#include "glm/glm.hpp"

using namespace glm;


struct Physics2D {
    float speed = 0.f;
    vec2 velocity = vec2(0.f, 0.f);
    vec2 acceleration = vec2(0.f, 0.f);
    float coefficientOfFriction = 0.f;
};