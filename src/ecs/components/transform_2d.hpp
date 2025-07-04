#pragma once

#include <glm/glm.hpp>

#include "utils/utils.hpp"

using namespace glm;

class Transform2D {
private:
    vec2 position = vec2(0.f, 0.f);
    float rotation = 0.f;
    vec2 scale = vec2(1.f, 1.f);
public:
    Transform2D() = default;
    Transform2D(vec2 position, float rotation, vec2 scale)
        : position(position), rotation(rotation), scale(scale) {}

    vec2 GetPosition() { return position; }
    float GetRotation() { return rotation; }
    vec2 GetScale() { return scale; }

    void SetPosition(vec2 position) { this->position = position; }
    void SetRotation(float rotation) { this->rotation = rotation; }
    void SetScale(vec2 scale) { this->scale = scale; }
};