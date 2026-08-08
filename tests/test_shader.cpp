#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <utility>

#include "graphics/shader/shader.hpp"

TEST_CASE("a shader owns its source stages")
{
    auto vertexShader = std::make_shared<VertexShader>("vertex source");
    auto fragmentShader = std::make_shared<FragmentShader>("fragment source");
    std::weak_ptr<VertexShader> vertexShaderObserver = vertexShader;
    std::weak_ptr<FragmentShader> fragmentShaderObserver = fragmentShader;

    Shader shader(vertexShader, fragmentShader);
    vertexShader.reset();
    fragmentShader.reset();

    REQUIRE_FALSE(vertexShaderObserver.expired());
    REQUIRE_FALSE(fragmentShaderObserver.expired());
    REQUIRE(shader.getVertexShader()->getSource() == "vertex source");
    REQUIRE(shader.getFragmentShader()->getSource() == "fragment source");
}

TEST_CASE("destroying a shader releases unshared source stages")
{
    std::weak_ptr<VertexShader> vertexShaderObserver;
    std::weak_ptr<FragmentShader> fragmentShaderObserver;

    {
        auto vertexShader = std::make_shared<VertexShader>("vertex source");
        auto fragmentShader = std::make_shared<FragmentShader>("fragment source");
        vertexShaderObserver = vertexShader;
        fragmentShaderObserver = fragmentShader;

        Shader shader(std::move(vertexShader), std::move(fragmentShader));
    }

    REQUIRE(vertexShaderObserver.expired());
    REQUIRE(fragmentShaderObserver.expired());
}
