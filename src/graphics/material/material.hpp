#pragma once

#include "graphics/shader/uniform.hpp"

class Material
{
public:
    Material() {
        this->setUniform("albedo", UniformType::COLOUR, Colour(255, 255, 255));
    };

    std::vector<ShaderUniform>& getUniforms() { return uniforms; }
    void setUniform(const std::string& name, UniformType type, UniformValue value)
    {
        uniforms.push_back({name, type, value});
    }

private:
    std::vector<ShaderUniform> uniforms;
};