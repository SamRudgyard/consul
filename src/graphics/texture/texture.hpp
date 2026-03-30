#pragma once

#include <map>
#include <string>

/**
 * Supported texture types used in shaders.
 */
enum TextureType {
    DIFFUSE,
    SPECULAR,
    NORMAL
};

class Texture
{
public:
    Texture();

    Texture(std::string texturePath, TextureType textureType);

    static Texture getDefaultDiffuseTexture();

    static Texture getDefaultSpecularTexture();

    const std::string& getPath() const { return path; }
    unsigned int getID() const { return id; }
    TextureType getType() const { return type; }
    const char* getTextureTypeAsString() const { return textureTypeToString.at(type); }

    const bool operator==( const Texture& other ) const;

private:
    unsigned int id = 0;
    std::string path;
    TextureType type;

    // Textures will be named "diffuse0", "diffuse1", "specular0", "specular1", etc.
    std::map <TextureType, const char*> textureTypeToString = {
        {TextureType::DIFFUSE, "diffuse"},
        {TextureType::SPECULAR, "specular"},
        {TextureType::NORMAL, "normal"}
    };
};
