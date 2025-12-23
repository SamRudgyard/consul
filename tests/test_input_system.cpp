#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include "input/input_system.hpp"

TEST_CASE("input system forwards keyboard state", "[input][system]") {
  InputSystem input;
  const auto key = KeyboardKey::KEY_A;

  input.beginTick();
  input.setKeyDown(key);
  REQUIRE(input.isKeyDown(key));
  REQUIRE(input.isKeyPressed(key));

  input.beginTick();
  REQUIRE_FALSE(input.isKeyPressed(key));

  input.setKeyUp(key);
  REQUIRE_FALSE(input.isKeyDown(key));
}

TEST_CASE("input system forwards mouse buttons", "[input][system]") {
  InputSystem input;
  const auto button = MouseButton::BUTTON_LEFT;

  input.beginTick();
  input.setMouseButtonDown(button);
  REQUIRE(input.isMouseButtonDown(button));
  REQUIRE(input.isMouseButtonPressed(button));

  input.beginTick();
  REQUIRE_FALSE(input.isMouseButtonPressed(button));

  input.setMouseButtonUp(button);
  REQUIRE(input.isMouseButtonUp(button));
}

TEST_CASE("input system tracks mouse position across ticks", "[input][system]") {
  InputSystem input;

  input.setMousePosition({10.0f, 20.0f});
  input.endTick();
  const auto first = input.getPreviousMousePosition();
  REQUIRE(first.x == Catch::Approx(10.0f));
  REQUIRE(first.y == Catch::Approx(20.0f));

  input.setMousePosition({30.0f, 40.0f});
  input.endTick();
  const auto second = input.getPreviousMousePosition();
  REQUIRE(second.x == Catch::Approx(30.0f));
  REQUIRE(second.y == Catch::Approx(40.0f));
}

TEST_CASE("input system forwards mouse flags and scroll", "[input][system]") {
  InputSystem input;

  input.setMouseScrollOffset({1.5f, -2.5f});
  const auto scroll = input.getMouseScrollOffset();
  REQUIRE(scroll.x == Catch::Approx(1.5f));
  REQUIRE(scroll.y == Catch::Approx(-2.5f));

  input.setMouseInsideWindow(false);
  REQUIRE_FALSE(input.isMouseInsideWindow());

  input.setMouseVisibility(false);
  REQUIRE_FALSE(input.getMouseVisibility());
}
