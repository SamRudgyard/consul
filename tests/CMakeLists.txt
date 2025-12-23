cmake_minimum_required(VERSION 3.24)

include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.5.4
)
FetchContent_MakeAvailable(Catch2)

add_executable(consul_tests
  test_constants.cpp
  test_input_system.cpp
  test_keyboard_state.cpp
  test_mouse_state.cpp
  test_unit_conversions.cpp
)

target_link_libraries(consul_tests
  PRIVATE
    Catch2::Catch2WithMain
    glm::glm
)

target_include_directories(consul_tests
  PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

include(CTest)
include(Catch)
catch_discover_tests(consul_tests)
