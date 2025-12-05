#pragma once

#include "../texture.hpp"
#include "graphics/shaders/shader.hpp"

class OpenGLTexture : public ITexture
{
public:
    OpenGLTexture(const char* texturePath, TextureType textureType);

    void bind() const override;
    void unbind() const override;
    void release() const override;
    void setTextureUnit(const IShader* shader, const char* uniform) const override;

private:
    unsigned int id;
    unsigned int unit;
};