#pragma once

#include "SDL3/SDL.h"
#include "glm/glm.hpp"
#include "utils/utils.hpp"

using namespace glm;

class Rectangle {
private:
    float width;
    float height;
    vec4 colour;
    bool isFilled = true;
    bool isCentred = true;
public:
    Rectangle(int width, int height, vec4 colour) {
        this->width = width;
        this->height = height;
        this->colour = colour;
    };
    void Draw(SDL_Renderer* renderer, vec2 position) {
        Log("Drawing Rectangle...");
        if (isCentred) position -= vec2(width/2, height/2);
        SDL_FRect rect = {position.x, position.y, width, height};
        SDL_SetRenderDrawColor(renderer, colour.x, colour.y, colour.z, colour.w);
        SDL_RenderFillRect(renderer, &rect);
    }
};