#pragma once

#include "core/models/texture.hpp"
#include "glm/glm.hpp"

/** Simple 2D sprite: position/size and an associated texture. */
class Sprite
{
public:
    glm::vec2 position;
    glm::vec2 size;
    Texture* texture;
};