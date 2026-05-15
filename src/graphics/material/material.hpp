#pragma once

#include "graphics/shader/uniform.hpp"

class Material
{
public:
    Material() = default;

    std::vector<ShaderUniform>& getUniforms() { return uniforms; }
    void addUniform(const std::string& name, UniformType type, UniformValue value)
    {
        uniforms.push_back({name, type, value});
    }

private:
    std::vector<ShaderUniform> uniforms;
};