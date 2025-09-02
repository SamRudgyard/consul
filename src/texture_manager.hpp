#pragma once

#include <map>
#include <memory>
#include <string>

#include "texture.hpp"

class TextureManager
{
public:
    static std::map<std::string, std::shared_ptr<Texture>> textures;

    static std::shared_ptr<Texture> LoadTexture(const char* filePath, const char* referenceName, bool requiresAlphaChannel = false);
    static std::shared_ptr<Texture> GetTexture(const char* referenceName);
    static void UnloadTexture(std::shared_ptr<Texture> texture);
    static void Clear();
private:
    TextureManager() {} // Private constructor as all methods are static

};
