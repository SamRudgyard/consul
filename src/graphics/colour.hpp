#pragma once

#include <cstdint>

#include <glm/glm.hpp>

struct Colour
{
    std::uint8_t r;
    std::uint8_t g;
    std::uint8_t b;
    std::uint8_t alpha;

    Colour()
        : r(0), g(0), b(0), alpha(255)
    {}

    Colour(std::uint8_t red, std::uint8_t green, std::uint8_t blue, std::uint8_t a = 255)
        : r(red), g(green), b(blue), alpha(a)
    {}

    bool operator==(const Colour& other) const
    {
        return r == other.r && g == other.g && b == other.b && alpha == other.alpha;
    }

    bool operator!=(const Colour& other) const
    {
        return !(*this == other);
    }

    glm::vec4 toVec4() const
    {
        return glm::vec4(r / 255.0f, g / 255.0f, b / 255.0f, alpha / 255.0f);
    }
};
