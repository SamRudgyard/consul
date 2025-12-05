#pragma once

#include "../renderable_texture.hpp"
#include "graphics/shaders/shader.hpp"

class OpenGLTexture : public RenderableTexture
{
public:
    OpenGLTexture(const TextureData& textureData);

    void bind() const override;
    void unbind() const override;
    void release() const override;
    void setTextureUnit(const IShader* shader, const char* uniform) const override;

private:
    unsigned int id;
    unsigned int unit;
};