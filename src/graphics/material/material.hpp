#pragma once

#include "graphics/shader/uniform.hpp"
#include "graphics/texture/texture.hpp"

#include <algorithm>
#include <utility>
#include <vector>

class Material
{
public:
    Material() {
        this->setUniform("albedo", Colour(255, 255, 255));
        this->setTexture(Texture::getDefaultDiffuseTexture());
        this->setTexture(Texture::getDefaultSpecularTexture());
    };

    std::vector<ShaderUniform>& getUniforms() { return uniforms; }
    const std::vector<ShaderUniform>& getUniforms() const { return uniforms; }

    void setUniform(const std::string& name, UniformValue value)
    {
        uniforms.push_back({name, value});
    }

    std::vector<Texture>& getTextures() { return textures; }
    const std::vector<Texture>& getTextures() const { return textures; }

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
    std::vector<ShaderUniform> uniforms;
    std::vector<Texture> textures;
};
