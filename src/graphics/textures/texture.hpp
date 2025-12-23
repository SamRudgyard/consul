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
    Texture(std::string texturePath, TextureType textureType) 
        : path(texturePath), type(textureType) {};

    const std::string& getPath() const { return path; }
    TextureType getType() const { return type; }
    const char* getTextureTypeAsString() const { return textureTypeToString.at(type); }
private:
    std::string path;
    TextureType type;

    // Textures will be named "diffuse0", "diffuse1", "specular0", "specular1", etc.
    std::map <TextureType, const char*> textureTypeToString = {
        {TextureType::DIFFUSE, "diffuse"},
        {TextureType::SPECULAR, "specular"},
        {TextureType::NORMAL, "normal"}
    };
};
