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

Shader::Shader(AssetID vertexShaderID, AssetID fragmentShaderID)
    : vertexShaderID(vertexShaderID),
      fragmentShaderID(fragmentShaderID)
{
}
