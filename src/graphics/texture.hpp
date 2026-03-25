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
    Texture()
        : Texture("assets/default/default.png", TextureType::DIFFUSE) {}

    Texture(std::string texturePath, TextureType textureType)
        : path(texturePath), type(textureType)
    {
        static unsigned int nextID = 0;
        id = nextID++;
    }

    static Texture getDefaultDiffuseTexture() {
        static std::string defaultTexturePath = "assets/default/default.png";
        return Texture(defaultTexturePath, TextureType::DIFFUSE);
    }

    static Texture getDefaultSpecularTexture() {
        static std::string defaultTexturePath = "assets/default/default.png";
        return Texture(defaultTexturePath, TextureType::SPECULAR);
    }

    const std::string& getPath() const { return path; }
    unsigned int getID() const { return id; }
    TextureType getType() const { return type; }
    const char* getTextureTypeAsString() const { return textureTypeToString.at(type); }

    // Texture operator= (const Texture& other)
    // {
    //     path = other.path;
    //     type = other.type;
    //     return *this;
    // }

    const bool operator==( const Texture& other ) const
    {
        return path == other.path && type == other.type;
    }
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
