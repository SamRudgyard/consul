#include "sprite_sheet.hpp"

#include "core/console/console.hpp"
#include "texture_manager.hpp"
#include "utils.hpp"

SpriteSheet::SpriteSheet(const char* filePath, int spriteWidth, int spriteHeight, bool requiresAlphaChannel)
{
    texture = TextureManager::LoadTexture(filePath, filePath, requiresAlphaChannel);

    int nSpritesX = texture->width/spriteWidth;
    int nSpritesY = texture->height/spriteHeight;

    for (int iy = 0; iy < nSpritesY; ++iy) {
        for (int ix = 0; ix < nSpritesX; ++ix) {
            Sprite sprite;
            sprite.position = glm::vec2(ix*spriteWidth, iy*spriteHeight);
            sprite.size = glm::vec2(spriteWidth, spriteHeight);
            sprite.texture = texture.get();
            sprites.push_back(sprite);
        }
    }

    Console::Get().LogOnDebug("[SpriteSheet::SpriteSheet] Loaded sprite sheet from file '" + std::string(filePath) + "' with " + std::to_string(sprites.size()) + " sprites.");
}

SpriteSheet::~SpriteSheet()
{
    TextureManager::UnloadTexture(texture);
}

Sprite SpriteSheet::GetSprite(int index) const
{
    if (index < 0 || index >= sprites.size()) {
        Console::Get().Error("[SpriteSheet::GetSprite] Invalid sprite index: " + std::to_string(index));
        return Sprite();
    }

    return sprites[index];
}
