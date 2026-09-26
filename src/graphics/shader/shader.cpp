#include "shader.hpp"

#include <utility>

#include "utils.hpp"

VertexShader::VertexShader()
    : source(getDefaultVertexShaderSource())
{
}

VertexShader::VertexShader(const std::string& source)
    : source(source)
{
}

std::string VertexShader::getDefaultVertexShaderSource() 
{
    const std::string shadersDirectory = SHADERS_DIR;
    const std::string defaultVertexFileName = "default_vertex_3d.glsl";
    return readFile(shadersDirectory + "/" + defaultVertexFileName);
}

FragmentShader::FragmentShader()
    : source(getDefaultFragmentShaderSource())
{
}

FragmentShader::FragmentShader(const std::string& source)
    : source(source)
{
}

std::string FragmentShader::getDefaultFragmentShaderSource() 
{
    const std::string shadersDirectory = SHADERS_DIR;
    const std::string defaultFragmentFileName = "default_fragment_3d.glsl";
    return readFile(shadersDirectory + "/" + defaultFragmentFileName);
}

Shader::Shader(
    const std::shared_ptr<VertexShader>& vertexShader,
    const std::shared_ptr<FragmentShader>& fragmentShader
) : vertexShader(vertexShader),
    fragmentShader(fragmentShader)
{
}
