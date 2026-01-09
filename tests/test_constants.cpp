#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include "maths/constants.hpp"

TEST_CASE("math constants have expected relationships", "[maths][constants]") {
  REQUIRE(TWO_PI == Catch::Approx(2.0f * PI));
  REQUIRE(EULERS_NUMBER == Catch::Approx(2.71828182845904523536f));
  REQUIRE(GOLDEN_RATIO * GOLDEN_RATIO == Catch::Approx(GOLDEN_RATIO + 1.0f));
}

TEST_CASE("math constants have sensible magnitudes", "[maths][constants]") {
  REQUIRE(MASSIVE > 1.0e30f);
  REQUIRE(TINY > 0.0f);
  REQUIRE(TINY < 1.0e-30f);
}
