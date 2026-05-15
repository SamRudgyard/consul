#pragma once

#include <string>
#include <variant>

#include "glm/glm.hpp"
#include "graphics/colour.hpp"

using UniformValue = std::variant<float, int, glm::vec2, glm::vec3, glm::vec4, Colour, glm::mat4>;

enum class UniformType
{
    FLOAT,
    INT,
    VEC2,
    VEC3,
    VEC4,
    COLOUR,
    MAT4
};

struct ShaderUniform
{
    std::string name;
    UniformType type;
    UniformValue value;
};