#include <catch2/catch_test_macros.hpp>

#include "core/project/scene.hpp"

TEST_CASE("destroying an uninitialised scene is safe")
{
    Scene scene;
}
