#pragma once

#include "core/ui/property_registry.hpp"
#include "graphics/shader/uniform.hpp"
#include "graphics/texture/texture.hpp"

#include <algorithm>
#include <utility>
#include <vector>

class Material
{
public:
    Material() {
        this->setTexture(Texture::getDefaultDiffuseTexture());
        this->setTexture(Texture::getDefaultSpecularTexture());
    };

    static void registerProperties()
    {
        PropertyRegistry& registry = PropertyRegistry::get();
        registry.registerProperty(PropertyInfo("Material", PropertyType::COLOUR, "Albedo"), &Material::getAlbedo, &Material::setAlbedo);
    }

    /**
     * Gets the shader uniforms associated with this material.
     * @returns Vector of shader uniforms.
     */
    std::vector<ShaderUniform>& getUniforms() { return uniforms; }

    /**
     * Gets the shader uniforms associated with this material.
     * @returns Vector of shader uniforms.
     */
    const std::vector<ShaderUniform>& getUniforms() const { return uniforms; }

    /**
     * Sets the albedo colour for this material.
     * @param colour Albedo colour to set.
     */
    void setAlbedo(Colour colour) { this->albedo = colour; }

    /**
     * Gets the albedo colour for this material.
     * @returns Albedo colour.
     */
    Colour getAlbedo() const { return albedo; }

    /**
     * Sets a shader uniform on this material.
     * @param name Name of the uniform.
     * @param value Value of the uniform.
     */
    void setUniform(const std::string& name, UniformValue value)
    {
        uniforms.push_back({name, value});
    }

    /**
     * Gets the textures associated with this material.
     * @returns Vector of textures.
     */
    std::vector<Texture>& getTextures() { return textures; }

    /**
     * Gets the textures associated with this material.
     * @returns Vector of textures.
     */
    const std::vector<Texture>& getTextures() const { return textures; }

    /**
     * Sets a texture for this material.
     * @param texture Texture to set.
     */
    void setTexture(Texture texture)
    {
        const TextureType textureType = texture.getType();
        textures.erase(
            std::remove_if(
                textures.begin(),
                textures.end(),
                [textureType](const Texture& existingTexture) {
                    return existingTexture.getType() == textureType;
                }
            ),
            textures.end()
        );
        textures.push_back(std::move(texture));
    }

private:
    Colour albedo = Colour(255, 255, 255);
    std::vector<ShaderUniform> uniforms;
    std::vector<Texture> textures;
};
