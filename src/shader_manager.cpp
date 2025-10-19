#include "shader_manager.hpp"

#include "core/console/console.hpp"
#include "utils.hpp"
#include "glad/glad.h"

std::map<std::string, Shader> ShaderManager::shaders = std::map<std::string, Shader>();

Shader ShaderManager::LoadShader(const char* vertexPath, const char* fragmentPath, const char* referenceName)
{
    Console& console = Console::Get();

    if (!vertexPath) console.Error("[ShaderManager::LoadShader] Vertex shader path is null");
    if (!fragmentPath) console.Error("[ShaderManager::LoadShader] Fragment shader path is null");

    // Check if name is already present
    if (shaders.find(referenceName) != shaders.end()) console.LogOnDebug("[ShaderManager::LoadShader] Shader with name '" + std::string(referenceName) + "' is already loaded, will overwrite.");

    char* vShaderText = ReadFile(vertexPath);
    char* fShaderText = ReadFile(fragmentPath);

    Shader shader;
    shader.Compile(vShaderText, fShaderText);

    UnloadFileText(vShaderText);
    UnloadFileText(fShaderText);

    shaders[referenceName] = shader;

    return shaders[referenceName];
}

Shader ShaderManager::GetShader(const char* referenceName)
{
    if (shaders.find(referenceName) == shaders.end()) {
        Console::Get().Error("[ShaderManager::GetShader] Shader with name '" + std::string(referenceName) + "' is not loaded");
        return Shader();
    }

    return shaders[referenceName];
}

void ShaderManager::UnloadShader(Shader shader)
{
    glDeleteProgram(shader.id);

    // Remove from map
    // Note: This requires that the shader was loaded with a reference name
    for (auto it = shaders.begin(); it != shaders.end(); ++it) {
        if (it->second.id == shader.id) {
            shaders.erase(it);
            break;
        }
    }

    Console::Get().LogOnDebug("[ShaderManager::UnloadShader] Unloaded shader program with ID " + std::to_string(shader.id));
}

void ShaderManager::Clear()
{
    for (auto& pair : shaders) {
        glDeleteProgram(pair.second.id);
    }
    shaders.clear();
    Console::Get().LogOnDebug("[ShaderManager::Clear] Cleared all shaders from memory");
}