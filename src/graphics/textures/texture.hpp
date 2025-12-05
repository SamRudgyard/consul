#pragma once

#include <map>
#include <string>

#include "graphics/shaders/shader.hpp"

/**
 * Supported texture types used in shaders.
 */
enum TextureType {
    DIFFUSE,
    SPECULAR
};

class ITexture {
public:
    /**
     * Bind this texture.
     */
    virtual void bind() const = 0;

    /**
     * Unbind this texture so another can be bound.
     */
    virtual void unbind() const = 0;

    /**
     * Release the texture.
     */
    virtual void release() const = 0;

    /**
     * Get the texture type.
     * @returns Texture type.
     */
    TextureType getTextureType() const { return type; }

    /**
     * Get the texture type as a string.
     * @returns String representation of the texture type.
     */
    const char* getTextureTypeAsString() const { return textureTypeToString.at(type); }

    /**
     * Set sampler uniform in a shader program to this texture's unit.
     * @param shader   Shader program.
     * @param uniform  Name of the sampler uniform variable.
     */
    virtual void setTextureUnit(const IShader* shader, const char* uniform) const = 0;

protected:
    /**
     * Loads a 2D texture. Generates mipmaps and sets repeat wrapping and linear filtering.
     * @param image       Path to image file.
     * @param textureType Texture type.
     */
    ITexture(const char* texturePath, TextureType textureType) 
        : path(texturePath), type(textureType) {};

private:
    std::string path;
    TextureType type;

    // Textures will be named "diffuse0", "diffuse1", "specular0", "specular1", etc.
    std::map <TextureType, const char*> textureTypeToString = {
        {TextureType::DIFFUSE, "diffuse"},
        {TextureType::SPECULAR, "specular"}
    };
};