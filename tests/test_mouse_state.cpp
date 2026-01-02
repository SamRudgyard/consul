#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include "input/mouse_state.hpp"

TEST_CASE("mouse button state transitions", "[input][mouse]") {
  MouseState state;
  const auto button = MouseButton::BUTTON_LEFT;

  state.beginTick();
  REQUIRE(state.isButtonUp(button));
  REQUIRE(state.wasButtonUp(button));

  state.setButtonDown(button);
  REQUIRE(state.isButtonDown(button));
  REQUIRE(state.isButtonPressed(button));

  state.beginTick();
  REQUIRE(state.wasButtonDown(button));
  REQUIRE_FALSE(state.isButtonPressed(button));

  state.setButtonUp(button);
  REQUIRE(state.isButtonUp(button));
}

TEST_CASE("mouse position and scroll tracking", "[input][mouse]") {
  MouseState state;

  state.setMousePosition({10.0f, 20.0f});
  state.endTick();
  const auto previous = state.getPreviousMousePosition();
  REQUIRE(previous.x == Catch::Approx(10.0f));
  REQUIRE(previous.y == Catch::Approx(20.0f));

  state.setScrollOffset({1.5f, -2.0f});
  const auto scroll = state.getScrollOffset();
  REQUIRE(scroll.x == Catch::Approx(1.5f));
  REQUIRE(scroll.y == Catch::Approx(-2.0f));
}

TEST_CASE("mouse visibility and window presence flags", "[input][mouse]") {
  MouseState state;
  REQUIRE(state.getMouseVisibility());
  REQUIRE(state.getIsInsideWindow());

  state.setMouseVisibility(false);
  state.setIsInsideWindow(false);
  REQUIRE_FALSE(state.getMouseVisibility());
  REQUIRE_FALSE(state.getIsInsideWindow());
}
