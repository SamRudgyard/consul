#pragma once

#include "glm/glm.hpp"

using namespace glm;

struct CollisionEdgeOfScreen {
    bool isEnabled = true;
    float reflectionMultiplier = -1.f;
};

struct RectCollider {
    vec2 offset = vec2(0.f, 0.f);
    vec2 size = vec2(0.f, 0.f);

    RectCollider() {}

    RectCollider(vec2 size, bool iscentered = true) {
        if (iscentered) {
            this->offset = -size/2.f;
        }
        this->size = size;
    }
};