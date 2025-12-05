#include "opengl_texture.hpp"

#include "core/console/console.hpp"
#include "glad/glad.h"
#include "utils.hpp"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif
#include <stb_image.h>

OpenGLTexture::OpenGLTexture(const char* texturePath, TextureType textureType)
    : ITexture(texturePath, textureType)
{
    if (!doesFileExist(texturePath)) {
        Console::get().error("[OpenGLTexture::OpenGLTexture] Texture file does not exist: '" + std::string(texturePath) + "'");
        return;
    }

    int textureWidth, textureHeight, numColourChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(texturePath, &textureWidth, &textureHeight, &numColourChannels, 0);
    if (!data) {
        Console::get().error("[OpenGLTexture::OpenGLTexture] Texture file could not be loaded: '" + std::string(texturePath) + "'");
        return;
    }

    glGenTextures(1, &id);
    bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // Set texture wrapping to GL_REPEAT along the X axis
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // Set texture wrapping to GL_REPEAT along the Y axis
    glCheckError();

    // Attach the image data to the currently bound texture (depending on the number of colour channels)
    if (numColourChannels == 4) { // RGBA
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    }
    else if (numColourChannels == 3) { // RGB
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    }
    else if (numColourChannels == 1) { // Grayscale - lets assign to the red channel
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, textureWidth, textureHeight, 0, GL_RED, GL_UNSIGNED_BYTE, data);
    }
    else {
        Console::get().error("[OpenGLTexture::OpenGLTexture] Invalid number of colour channels (expected 1, 3, or 4, but got " + std::to_string(numColourChannels) + ")");
        return;
    }

    glGenerateMipmap(GL_TEXTURE_2D); // Generate mipmaps (lower resolution copies of the texture for distance-based filtering)
    glCheckError();

    stbi_image_free(data);
    unbind();
    glCheckError();
}

void OpenGLTexture::bind() const {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, id);
}

void OpenGLTexture::unbind() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}

void OpenGLTexture::release() const {
    glDeleteTextures(1, &id);
}

void OpenGLTexture::setTextureUnit(const IShader* shader, const char* uniform) const {
    shader->use();
    shader->setUniformInt(uniform, unit);
    glUseProgram(0);
    glCheckError();
}