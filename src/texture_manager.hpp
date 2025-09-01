#pragma once

#include <map>
#include <string>

#include "texture.hpp"

class TextureManager
{
public:
    static std::map<std::string, Texture> textures;

    static Texture LoadTexture(const char* filePath, const char* referenceName, bool requiresAlphaChannel = false);
    static Texture GetTexture(const char* referenceName);
    static void UnloadTexture(Texture texture);
    static void Clear();
private:
    TextureManager() {} // Private constructor as all methods are static

};
