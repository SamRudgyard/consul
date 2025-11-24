#pragma once

#include "glm/glm.hpp"
#include <string>

/** Thin wrapper for an OpenGL shader program with convenience uniform setters. */
class Shader
{
public:
    unsigned int id;

    /**
     * Activates this shader program.
     * 
     * @returns Reference to this Shader object.
     */
    Shader& use();

    /**
     * Compile and link a shader program from provided vertex/fragment GLSL sources.
     * 
     * @param vertexSource   GLSL source code for the vertex shader.
     * @param fragmentSource GLSL source code for the fragment shader.
     */
    void compile(const std::string& vertexSource, const std::string& fragmentSource);

    /**
     * Set an integer uniform.
     * 
     * @param name  Name of the uniform variable in the shader.
     * @param value Integer value to set.
     */
    void setUniformInt(const char* name, int value);

    /**
     * Set a float uniform.
     * 
     * @param name  Name of the uniform variable in the shader.
     * @param value Float value to set.
     */
    void setUniformFloat(const char* name, float value);

    /**
     * Set a vec2 uniform from components.
     * 
     * @param name Name of the uniform variable in the shader.
     * @param x    X component of the vec2.
     * @param y    Y component of the vec2.
     */
    void setUniformVector2f(const char* name, const float x, const float y);
    
    /**
     * Set a vec2 uniform.
     * 
     * @param name  Name of the uniform variable in the shader.
     * @param value vec2 value to set.
     */
    void setUniformVector2f(const char* name, const glm::vec2& value);

    /**
     * Set a vec3 uniform from components.
     * 
     * @param name Name of the uniform variable in the shader.
     * @param x    X component of the vec3.
     * @param y    Y component of the vec3.
     * @param z    Z component of the vec3.
     */
    void setUniformVector3f(const char* name, const float x, const float y, const float z);

    /**
     * Set a vec3 uniform.
     * 
     * @param name  Name of the uniform variable in the shader.
     * @param value vec3 value to set.
     */
    void setUniformVector3f(const char* name, const glm::vec3& value);

    /**
     * Set a vec4 uniform from components.
     * 
     * @param name Name of the uniform variable in the shader.
     * @param x    X component of the vec4.
     * @param y    Y component of the vec4.
     * @param z    Z component of the vec4.
     * @param w    W component of the vec4.
     */
    void setUniformVector4f(const char* name, const float x, const float y, const float z, const float w);

    /**
     * Set a vec4 uniform.
     * 
     * @param name  Name of the uniform variable in the shader.
     * @param value vec4 value to set.
     */
    void setUniformVector4f(const char* name, const glm::vec4& value);

    /**
     * Set a mat4 uniform.
     * 
     * @param name   Name of the uniform variable in the shader.
     * @param matrix mat4 value to set.
     */
    void setUniformMatrix4f(const char* name, const glm::mat4& matrix);
};