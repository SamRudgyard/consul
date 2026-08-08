#pragma once

#include "core/ui/property_registry.hpp"
#include "graphics/colour.hpp"
#include "graphics/shader/uniform.hpp"

#include <memory>
#include <utility>
#include <vector>

class Texture;

class Material
{
public:
    Material() = default;

    /**
     * Registers the properties of the Material class, allowing them to be edited in the UI.
     */
    static void registerProperties()
    {
        PropertyRegistry& registry = PropertyRegistry::get();
        registry.registerProperty(PropertyInfo("Material", PropertyType::COLOUR, "Albedo"), &Material::getAlbedo, &Material::setAlbedo);
    }

    /**
     * Gets the shader uniforms associated with this material.
     * @returns Vector of shader uniforms.
     */
    std::vector<ShaderUniform> getUniforms()
    {
        std::vector<ShaderUniform> uniforms;
        uniforms.push_back({"albedo", albedo});
        return uniforms;
    }

    /**
     * Sets the albedo colour for this material.
     * @param colour Albedo colour to set.
     */
    void setAlbedo(Colour colour) { albedo = colour; }

    /**
     * Gets the albedo colour for this material.
     * @returns Albedo colour.
     */
    Colour getAlbedo() const { return albedo; }

    /**
     * Sets the albedo texture of this material.
     * @param texture Albedo texture to set.
     */
    void setAlbedoTexture(std::shared_ptr<Texture> texture) { albedoTexture = std::move(texture); }

    /**
     * Gets the albedo texture of this material.
     * @returns Albedo texture.
     */
    std::shared_ptr<Texture> getAlbedoTexture() const { return albedoTexture; }

    /**
     * Sets the specular texture of this material.
     * @param texture Specular texture to set.
     */
    void setSpecularTexture(std::shared_ptr<Texture> texture) { specularTexture = std::move(texture); }

    /**
     * Gets the specular texture of this material.
     * @returns Specular texture.
     */
    std::shared_ptr<Texture> getSpecularTexture() const { return specularTexture; }

    /**
     * Sets the normal texture of this material.
     * @param texture Normal texture to set.
     */
    void setNormalTexture(std::shared_ptr<Texture> texture) { normalTexture = std::move(texture); }

    /**
     * Gets the normal texture of this material.
     * @returns Normal texture.
     */
    std::shared_ptr<Texture> getNormalTexture() const { return normalTexture; }

private:
    Colour albedo = Colour(255, 255, 255);
    std::shared_ptr<Texture> albedoTexture;
    std::shared_ptr<Texture> specularTexture;
    std::shared_ptr<Texture> normalTexture;
};
