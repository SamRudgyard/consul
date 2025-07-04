#pragma once

#include <glm/glm.hpp>

#include "utils/utils.hpp"

using namespace glm;

class Transform2D {
public:
    vec2 position = vec2(0.f, 0.f);
    float rotation = 0.f;
    vec2 scale = vec2(1.f, 1.f);
};