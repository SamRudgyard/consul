#pragma once

#include "core/ui/property_registry.hpp"
#include "graphics/colour.hpp"
#include "graphics/shader/uniform.hpp"
#include "graphics/texture/texture.hpp"
#include "graphics/shader/shader.hpp"

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
    static void registerProperties();

    /**
     * Gets the shader uniforms associated with this material.
     * @returns Vector of shader uniforms.
     */
    std::vector<ShaderUniform> getUniforms();

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

    /**
     * Sets the shader associated with this material.
     * @param shader Shader to set.
     */
    void setShader(std::shared_ptr<Shader> shader) { this->shader = std::move(shader); }

    /**
     * Gets the shader associated with this material.
     * @returns Shader associated with this material.
     */
    std::shared_ptr<Shader> getShader() const { return shader; }

private:
    Colour albedo = Colour(255, 255, 255);
    std::shared_ptr<Texture> albedoTexture = std::make_shared<Texture>(TextureType::DIFFUSE);
    std::shared_ptr<Texture> specularTexture = std::make_shared<Texture>(TextureType::SPECULAR);
    std::shared_ptr<Texture> normalTexture = std::make_shared<Texture>(TextureType::NORMAL);
    std::shared_ptr<Shader> shader = std::make_shared<Shader>();
};
