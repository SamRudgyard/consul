#pragma once

#include "maths/constants.hpp"

// --------------------------------------------------
// Time
// --------------------------------------------------

constexpr float SECONDS_TO_MILLISECONDS = 1e3f;
constexpr float MILLISECONDS_TO_SECONDS = 1e-3f;
constexpr float SECONDS_TO_MICROSECONDS = 1e6f;
constexpr float MICROSECONDS_TO_SECONDS = 1e-6f;
constexpr float SECONDS_TO_NANOSECONDS  = 1e9f;
constexpr float NANOSECONDS_TO_SECONDS  = 1e-9f;

// --------------------------------------------------
// Length
// --------------------------------------------------

constexpr float METRES_TO_KILOMETRES  = 1e-3f;
constexpr float KILOMETRES_TO_METRES  = 1e3f;
constexpr float METRES_TO_CENTIMETRES = 1e2f;
constexpr float CENTIMETRES_TO_METRES = 1e-2f;

// --------------------------------------------------
// Angles
// --------------------------------------------------

constexpr float DEG_TO_RAD = PI / 180.0f;
constexpr float RAD_TO_DEG = 180.0f / PI;