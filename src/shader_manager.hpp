#pragma once

#include <map>
#include <string>

#include "shader.hpp"

/**
 * Shader Manager for managing Shader objects.
 */
class ShaderManager
{
public:
    static std::map<std::string, Shader> shaders;

    static Shader& LoadShader(const char* vertexPath, const char* fragmentPath, const char* referenceName);
    static Shader& GetShader(const char* referenceName);
    static void UnloadShader(Shader& shader);
    static void Clear();
private:
    ShaderManager() {} // Private constructor as all methods are static

};