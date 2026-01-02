#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include "maths/unit_conversions.hpp"

TEST_CASE("time conversions round-trip", "[maths][unit]") {
  const float seconds = 1.2345f;
  const float millis = seconds * SECONDS_TO_MILLISECONDS;
  REQUIRE(millis * MILLISECONDS_TO_SECONDS == Catch::Approx(seconds));

  const float micros = seconds * SECONDS_TO_MICROSECONDS;
  REQUIRE(micros * MICROSECONDS_TO_SECONDS == Catch::Approx(seconds));

  const float nanos = seconds * SECONDS_TO_NANOSECONDS;
  REQUIRE(nanos * NANOSECONDS_TO_SECONDS == Catch::Approx(seconds));
}

TEST_CASE("length conversions round-trip", "[maths][unit]") {
  const float metres = 123.4f;
  const float kilometres = metres * METRES_TO_KILOMETRES;
  REQUIRE(kilometres * KILOMETRES_TO_METRES == Catch::Approx(metres));

  const float centimetres = metres * METRES_TO_CENTIMETRES;
  REQUIRE(centimetres * CENTIMETRES_TO_METRES == Catch::Approx(metres));
}

TEST_CASE("angle conversions round-trip", "[maths][unit]") {
  const float degrees = 45.0f;
  const float radians = degrees * DEG_TO_RAD;
  REQUIRE(radians * RAD_TO_DEG == Catch::Approx(degrees));
}

TEST_CASE("angle conversion constants are reciprocal", "[maths][unit]") {
  REQUIRE(DEG_TO_RAD * RAD_TO_DEG == Catch::Approx(1.0f));
}
