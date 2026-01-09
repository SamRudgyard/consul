#pragma once

#include <cstdint>

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
};
