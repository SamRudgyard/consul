#pragma once

#include "../renderable_texture.hpp"
#include "graphics/shaders/shader.hpp"

class OpenGLTexture : public RenderableTexture
{
public:
    OpenGLTexture(const Texture& textureData);
    ~OpenGLTexture();

    void bind() const override;
    void unbind() const override;
    void release() const override;
    void setTextureUnit(const IShader* shader, const char* uniform) const override;

private:
    unsigned int id = 0;
    unsigned int unit = 0;
};
