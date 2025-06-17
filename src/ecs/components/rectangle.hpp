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
    Rectangle() {
        this->width = 0;
        this->height = 0;
        this->colour = vec4(255, 255, 255, 255);
    }
    Rectangle(int width, int height, vec4 colour) {
        this->width = width;
        this->height = height;
        this->colour = colour;
    }

    float GetWidth() { return width; }
    float GetHeight() { return height; }
    vec4 GetColour() { return colour; }
    bool IsFilled() { return isFilled; }
    bool IsCentred() { return isCentred; }

    void SetWidth(float width) { this->width = width; }
    void SetHeight(float height) { this->height = height; }
    void SetColour(vec4 colour) { this->colour = colour; }
    void SetFilled(bool isFilled) { this->isFilled = isFilled; }
    void SetCentred(bool isCentred) { this->isCentred = isCentred; }
};