#pragma once

#include <string>
#include <variant>

#include "core/console/console.hpp"
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
    UniformValue value;

    UniformType getType() const
    {
        if (std::holds_alternative<float>(value)) {
            return UniformType::FLOAT;
        }
        else if (std::holds_alternative<int>(value)) {
            return UniformType::INT;
        }
        else if (std::holds_alternative<glm::vec2>(value)) {
            return UniformType::VEC2;
        }
        else if (std::holds_alternative<glm::vec3>(value)) {
            return UniformType::VEC3;
        }
        else if (std::holds_alternative<glm::vec4>(value)) {
            return UniformType::VEC4;
        }
        else if (std::holds_alternative<Colour>(value)) {
            return UniformType::COLOUR;
        }
        else if (std::holds_alternative<glm::mat4>(value)) {
            return UniformType::MAT4;
        }
        else {
            Console::get().error("[ShaderUniform::getType] Unsupported uniform type '" + name + "'");
        }
    }
};