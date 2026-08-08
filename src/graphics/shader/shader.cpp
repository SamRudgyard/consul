#include "shader.hpp"

#include <utility>

VertexShader::VertexShader(std::string source)
    : source(std::move(source))
{
}

FragmentShader::FragmentShader(std::string source)
    : source(std::move(source))
{
}

Shader::Shader(
    std::shared_ptr<VertexShader> vertexShader,
    std::shared_ptr<FragmentShader> fragmentShader
) : vertexShader(std::move(vertexShader)),
    fragmentShader(std::move(fragmentShader))
{
}
