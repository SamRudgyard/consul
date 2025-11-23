#pragma once

#include <map>
#include <string>

#include "shader.hpp"

/** Manager for loading, caching, retrieving and unloading Shader programs. */
class ShaderManager
{
public:
    static std::map<std::string, Shader> shaders;

    /**
     * Loads, compiles and links a shader program. The shader is cached with the given reference name.
     * If an entry with the same name exists it will be overwritten.
     * 
     * @param vertexPath     Path to the vertex shader GLSL source file.
     * @param fragmentPath   Path to the fragment shader GLSL source file.
     * @param referenceName  Reference name to cache the shader under.
     * @returns Reference to the loaded and compiled Shader object.
     */
    static Shader& LoadShader(const char* vertexPath, const char* fragmentPath, const char* referenceName);

    /**
     * Retrieve a previously loaded shader by reference name.
     * @param referenceName Reference name of the shader.
     * @returns Reference to the Shader object.
     */
    static Shader& GetShader(const char* referenceName);

    /**
     * Delete the GL program and remove it from the cache map.
     * @param shader Reference to the Shader object to unload.
     */
    static void UnloadShader(Shader& shader);

    /**
     * Delete all GL programs and clear the cache.
     */
    static void Clear();
private:
    ShaderManager() {} // Private constructor as all methods are static

};