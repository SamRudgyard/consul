#include "texture.hpp"

#include "core/console/console.hpp"

Texture::Texture(const TextureType& textureType)
    : type(textureType)
{
    path = getDefaultTexturePath();
}

Texture::Texture(std::string texturePath, TextureType textureType)
    : path(texturePath), type(textureType)
{
}

const std::string& Texture::getTextureTypeAsString() const
{
    static const std::map<TextureType, std::string> textureTypeToString = {
        {TextureType::DIFFUSE, "diffuse"},
        {TextureType::SPECULAR, "specular"},
        {TextureType::NORMAL, "normal"}
    };

    auto it = textureTypeToString.find(type);
    if (it == textureTypeToString.end()) {
        Console::get().error("[Texture::getTextureTypeAsString] Unknown texture type!");
    }

    return it->second;
}

const bool Texture::operator==( const Texture& other ) const
{
    return path == other.path && type == other.type;
}


std::string Texture::getDefaultTexturePath()
{
    const std::string assetsDirectory = ASSETS_DIR;
    return assetsDirectory + "/default/default.png";
}