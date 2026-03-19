#pragma once

#include "../renderable_texture.hpp"
#include "graphics/shader.hpp"

class OpenGLTexture : public RenderableTexture
{
public:
    OpenGLTexture(const Texture& textureData, const unsigned int unit);
    ~OpenGLTexture();

    void bind() const override;
    void unbind() const override;
    void release() const override;
    void setTextureUnit(const Shader* shader, const char* uniform) const override;

private:
    unsigned int id = 0;
    unsigned int unit = 0;
};
