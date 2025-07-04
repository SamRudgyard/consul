#pragma once

#include "SDL3/SDL.h"
#include "glm/glm.hpp"
#include "utils/utils.hpp"

using namespace glm;

class Rectangle {
public:
    float width;
    float height;
    vec4 colour;
    bool isFilled = true;
    bool isCentred = true;
};