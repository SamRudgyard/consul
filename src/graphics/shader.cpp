#include "shader.hpp"
#include "utils.hpp"

Shader::Shader(const std::string& vertexFilePath, const std::string& fragmentFilePath)
{
    static unsigned int nextID = 0;
    id = nextID++;

    vertexSource = readFile(vertexFilePath.c_str());
    fragmentSource = readFile(fragmentFilePath.c_str());
}