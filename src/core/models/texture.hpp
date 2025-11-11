#pragma once

#include "glad/glad.h"

#include <map>
#include <string>

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
        
        Texture(const char* image, TextureType textureType, GLuint unit);

        const char* GetTextureTypeAsString() const { return textureTypeToString.at(type); }

        void SetTextureUnit(unsigned int shaderID, const char* uniform) const;
        void Bind() const;
        void Unbind() const;
        void Delete() const;

    private:
        // Textures will be named "diffuse0", "diffuse1", "specular0", "specular1", etc.
        std::map <TextureType, const char*> textureTypeToString = {
            {TextureType::DIFFUSE, "diffuse"},
            {TextureType::SPECULAR, "specular"}
        };
};