#pragma once

#include <memory>
#include <vector>

#include "texture.hpp"
#include "sprite.hpp"

class SpriteSheet
{
public:
    SpriteSheet(const char* filePath, int spriteWidth, int spriteHeight, bool requiresAlphaChannel = true);
    ~SpriteSheet();

    Sprite GetSprite(int index) const;
private:
    std::shared_ptr<Texture> texture;
    std::vector<Sprite> sprites;
};