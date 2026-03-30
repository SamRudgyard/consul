#include "texture.hpp"

Texture::Texture()
    : Texture("assets/default/default.png", TextureType::DIFFUSE)
{

}

Texture::Texture(std::string texturePath, TextureType textureType)
    : path(texturePath), type(textureType)
{
    static unsigned int nextID = 0;
    id = nextID++;
}

Texture Texture::getDefaultDiffuseTexture()
{
    static std::string defaultTexturePath = "assets/default/default.png";
    return Texture(defaultTexturePath, TextureType::DIFFUSE);
}

Texture Texture::getDefaultSpecularTexture()
{
    static std::string defaultTexturePath = "assets/default/default.png";
    return Texture(defaultTexturePath, TextureType::SPECULAR);
}

const bool Texture::operator==( const Texture& other ) const
{
    return path == other.path && type == other.type;
}