#pragma once

#include <map>
#include <string>

#include "graphics/shaders/shader.hpp"
#include "graphics/textures/texture.hpp"

class RenderableTexture {
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
    RenderableTexture(const Texture& textureData)
        : textureData(textureData) {};

private:
    Texture textureData;
};