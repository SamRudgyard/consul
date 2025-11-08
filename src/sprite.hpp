#pragma once

#include "core/models/texture.hpp"
#include "glm/glm.hpp"

class Sprite
{
public:
    glm::vec2 position;
    glm::vec2 size;
    Texture* texture;
};