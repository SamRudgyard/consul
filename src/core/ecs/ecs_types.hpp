#pragma once

#include <bitset>
#include <cstdint>

using Entity = std::uint32_t;
inline constexpr Entity MAX_ENTITIES = 5000;
using ComponentType = std::uint32_t;
inline constexpr ComponentType MAX_COMPONENTS = 32;
using ComponentMask = std::bitset<MAX_COMPONENTS>;
