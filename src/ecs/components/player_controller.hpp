#pragma once

#include "glm/glm.hpp"

using namespace glm;

class PlayerController {
private:
    vec2 inputDirection = vec2(0.f, 0.f);
public:
    vec2 GetInputDirection() { return inputDirection; }
    void SetInputDirection(vec2 direction) { this->inputDirection = direction; }
};