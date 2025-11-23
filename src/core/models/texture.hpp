#pragma once

#include "glad/glad.h"

#include <map>
#include <string>

/**
 * Supported texture types used in shaders.
 */
enum TextureType {
    DIFFUSE,
    SPECULAR
};

class Texture {
    public:
        std::string path;
        GLuint id;
        TextureType type;
        GLuint unit;
        /**
         * Loads a 2D texture. Generates mipmaps and sets repeat wrapping and linear filtering.
         * @param image       Path to image file.
         * @param textureType Texture type.
         * @param unit        Texture unit to which this texture will bind when used.
         */
        Texture(const char* image, TextureType textureType, GLuint unit);

        /**
         * Get the texture type as a string.
         * @returns String representation of the texture type.
         */
        const char* GetTextureTypeAsString() const { return textureTypeToString.at(type); }

        /**
         * Set sampler uniform in a shader program to this texture's unit.
         * @param shaderID ID of the shader program.
         * @param uniform  Name of the sampler uniform variable.
         */
        void SetTextureUnit(unsigned int shaderID, const char* uniform) const;

        /**
         * Bind this texture.
         */
        void Bind() const;

        /**
         * Unbind this texture so another can be bound.
         */
        void Unbind() const;

        /**
         * Delete the texture.
         */
        void Delete() const;

    private:
        // Textures will be named "diffuse0", "diffuse1", "specular0", "specular1", etc.
        std::map <TextureType, const char*> textureTypeToString = {
            {TextureType::DIFFUSE, "diffuse"},
            {TextureType::SPECULAR, "specular"}
        };
};