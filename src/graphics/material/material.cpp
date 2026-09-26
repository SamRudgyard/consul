#include "material.hpp"

void Material::registerProperties()
{
    PropertyRegistry& registry = PropertyRegistry::get();
    registry.registerProperty(PropertyInfo("Material", PropertyType::COLOUR, "Albedo"), &Material::getAlbedo, &Material::setAlbedo);
}

std::vector<ShaderUniform> Material::getUniforms()
{
    std::vector<ShaderUniform> uniforms;
    uniforms.push_back({"albedo", albedo});
    return uniforms;
}