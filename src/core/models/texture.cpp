#include "core/models/texture.hpp"
#include "core/console/console.hpp"
#include "utils.hpp"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif
#include <stb_image.h>

#include <fstream>
#include <iostream>

Texture::Texture(const char* image, TextureType textureType, GLuint unit)
    : path(image), type(textureType), unit(unit)
{
    if (!DoesFileExist(image)) {
        Console::Get().Error("[Texture::Texture] Texture file does not exist: '" + std::string(image) + "'");
        return;
    }

    int textureWidth, textureHeight, numColourChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(image, &textureWidth, &textureHeight, &numColourChannels, 0);
    if (!data) {
        Console::Get().Error("[Texture::Texture] Texture file could not be loaded: '" + std::string(image) + "'");
        return;
    }

    glGenTextures(1, &id);
    Bind();
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
    else if (numColourChannels == 1) { // Grayscale
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, textureWidth, textureHeight, 0, GL_RED, GL_UNSIGNED_BYTE, data);
    }
    else {
        Console::Get().Error("[Texture::Texture] Invalid number of colour channels (expected 1, 3, or 4, but got " + std::to_string(numColourChannels) + ")");
        return;
    }

    glGenerateMipmap(GL_TEXTURE_2D); // Generate mipmaps (lower resolution copies of the texture for distance-based filtering)
    glCheckError();

    stbi_image_free(data);
    Unbind();
    glCheckError();
}

void Texture::SetTextureUnit(unsigned int shaderID, const char* uniform) const {
    GLuint textureUniformID = glGetUniformLocation(shaderID, uniform);
    glUseProgram(shaderID);
    glUniform1i(textureUniformID, unit);
    glCheckError();
}

void Texture::Bind() const {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, id);
}

void Texture::Unbind() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Delete() const {
    glDeleteTextures(1, &id);
}