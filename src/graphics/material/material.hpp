#pragma once

#include "core/project/asset_types.hpp"
#include "core/ui/property_registry.hpp"
#include "graphics/shader/uniform.hpp"
#include "graphics/texture/texture.hpp"

#include <algorithm>
#include <memory>
#include <utility>
#include <vector>

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
     * Sets the texture ID for the albedo texture of this material.
     * @param textureID AssetID of the albedo texture to set.
     */
    void setAlbedoTextureID(AssetID textureID) { albedoTextureID = textureID; }

    /**
     * Gets the texture ID for the albedo texture of this material.
     * @returns AssetID of the albedo texture.
     */
    AssetID getAlbedoTextureID() const { return albedoTextureID; }

    /**
     * Sets the texture ID for the specular texture of this material.
     * @param textureID AssetID of the specular texture to set.
     */
    void setSpecularTextureID(AssetID textureID) { specularTextureID = textureID; }

    /**
     * Gets the texture ID for the specular texture of this material.
     * @returns AssetID of the specular texture.
     */
    AssetID getSpecularTextureID() const { return specularTextureID; }

    /**
     * Sets the texture ID for the normal texture of this material.
     * @param textureID AssetID of the normal texture to set.
     */
    void setNormalTextureID(AssetID textureID) { normalTextureID = textureID; }

    /**
     * Gets the texture ID for the normal texture of this material.
     * @returns AssetID of the normal texture.
     */
    AssetID getNormalTextureID() const { return normalTextureID; }

private:
    Colour albedo = Colour(255, 255, 255);
    AssetID albedoTextureID = INVALID_ASSET_ID;
    AssetID specularTextureID = INVALID_ASSET_ID;
    AssetID normalTextureID = INVALID_ASSET_ID;
};
