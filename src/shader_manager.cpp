#include "shader_manager.hpp"

#include "core/console/console.hpp"
#include "utils.hpp"
#include "glad/glad.h"

std::map<std::string, Shader> ShaderManager::shaders = std::map<std::string, Shader>();

Shader& ShaderManager::loadShader(const char* vertexPath, const char* fragmentPath, const char* referenceName)
{
    Console& console = Console::get();

    if (!vertexPath) console.error("[ShaderManager::loadShader] Vertex shader path is null");
    if (!fragmentPath) console.error("[ShaderManager::loadShader] Fragment shader path is null");

    // Check if name is already present
    if (shaders.find(referenceName) != shaders.end()) console.logOnDebug("[ShaderManager::loadShader] Shader with name '" + std::string(referenceName) + "' is already loaded, will overwrite.");

    const std::string vShaderText = readFile(vertexPath);
    const std::string fShaderText = readFile(fragmentPath);

    Shader shader;
    shader.compile(vShaderText, fShaderText);

    shaders[referenceName] = shader;

    return shaders[referenceName];
}

Shader& ShaderManager::getShader(const char* referenceName)
{
    if (shaders.find(referenceName) == shaders.end()) {
        Console::get().error("[ShaderManager::getShader] Shader with name '" + std::string(referenceName) + "' is not loaded");
    }

    return shaders[referenceName];
}

void ShaderManager::unloadShader(Shader& shader)
{
    glDeleteProgram(shader.id);
    glCheckError();

    // Remove from map
    // Note: This requires that the shader was loaded with a reference name
    for (auto it = shaders.begin(); it != shaders.end(); ++it) {
        if (it->second.id == shader.id) {
            shaders.erase(it);
            break;
        }
    }

    Console::get().logOnDebug("[ShaderManager::unloadShader] Unloaded shader program with ID " + std::to_string(shader.id));
}

void ShaderManager::clear()
{
    for (auto& pair : shaders) {
        glDeleteProgram(pair.second.id);
    }
    glCheckError();

    shaders.clear();
    Console::get().logOnDebug("[ShaderManager::clear] Cleared all shaders from memory");
}