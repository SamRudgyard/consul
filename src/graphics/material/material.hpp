#pragma once

#include "graphics/shader/uniform.hpp"

class Material
{
public:
    Material() {
        this->setUniform("albedo", Colour(255, 255, 255));
    };

    std::vector<ShaderUniform>& getUniforms() { return uniforms; }
    void setUniform(const std::string& name, UniformValue value)
    {
        uniforms.push_back({name, value});
    }

private:
    std::vector<ShaderUniform> uniforms;
};