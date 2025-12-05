#pragma once

#include <map>
#include <string>

/**
 * Supported texture types used in shaders.
 */
enum TextureType {
    DIFFUSE,
    SPECULAR
};

class Texture
{
public:
    Texture(const char* texturePath, TextureType textureType) 
        : path(texturePath), type(textureType) {};

    const char* getPath() const { return path; }
    TextureType getType() const { return type; }
private:
    const char* path;
    TextureType type;

    // Textures will be named "diffuse0", "diffuse1", "specular0", "specular1", etc.
    std::map <TextureType, const char*> textureTypeToString = {
        {TextureType::DIFFUSE, "diffuse"},
        {TextureType::SPECULAR, "specular"}
    };
};