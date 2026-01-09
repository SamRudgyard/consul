#include <catch2/catch_test_macros.hpp>

#include "input/keyboard_state.hpp"

TEST_CASE("keyboard state tracks presses across ticks", "[input][keyboard]") {
  KeyboardState state;
  const auto key = KeyboardKey::KEY_A;

  state.beginTick();
  REQUIRE(state.isKeyUp(key));
  REQUIRE_FALSE(state.wasKeyDown(key));

  state.setKeyDown(key);
  REQUIRE(state.isKeyDown(key));
  REQUIRE(state.isKeyPressed(key));

  state.beginTick();
  REQUIRE(state.wasKeyDown(key));
  REQUIRE_FALSE(state.isKeyPressed(key));

  state.setKeyUp(key);
  REQUIRE(state.isKeyUp(key));
}

TEST_CASE("keyboard exit key defaults and can be overridden", "[input][keyboard]") {
  KeyboardState state;
  REQUIRE(state.getExitKey() == KeyboardKey::KEY_ESCAPE);

  state.setExitKey(KeyboardKey::KEY_Q);
  REQUIRE(state.getExitKey() == KeyboardKey::KEY_Q);
}
