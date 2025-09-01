#include "shader.hpp"

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "utils.hpp"

/**
 * @brief the shader program for use in the current OpenGL context.
 *
 * @return Reference to the current Shader object.
 */
Shader& Shader::Use()
{
    glUseProgram(id);
    return *this;
}

/**
 * @brief Compiles and links a shader program from given vertex and fragment shader source code.
 *
 * This function creates vertex and fragment shader objects, compiles them from the provided source code,
 * checks for compilation errors, and then links them into a shader program. On success, the shader
 * program is stored in the member variable 'id'. The individual shader objects are deleted after linking.
 *
 * @param vertexSource C-string containing the GLSL source code for the vertex shader.
 * @param fragmentSource C-string containing the GLSL source code for the fragment shader.
 */
void Shader::Compile(const char* vertexSource, const char* fragmentSource)
{
    // Create shaders
    unsigned int vertex, fragment;
    vertex = glCreateShader(GL_VERTEX_SHADER);
    fragment = glCreateShader(GL_FRAGMENT_SHADER);

    // Compile vertex shader
    glShaderSource(vertex, 1, &vertexSource, NULL);
    glCompileShader(vertex);
    // Check for compilation errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        Error("[Shader::Compile] Vertex shader compilation failed: " + std::string(infoLog));
    }

    LogOnDebug("[Shader::Compile] Vertex shader successfully compiled");

    // Compile fragment shader
    glShaderSource(fragment, 1, &fragmentSource, NULL);
    glCompileShader(fragment);
    // Check for compilation errors
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        Error("[Shader::Compile] Fragment shader compilation failed: " + std::string(infoLog));
    }

    LogOnDebug("[Shader::Compile] Fragment shader successfully compiled");

    // Create shader program and link shaders
    id = glCreateProgram();
    glAttachShader(id, vertex);
    glAttachShader(id, fragment);
    glLinkProgram(id);
    // Check for linking errors
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(id, 512, NULL, infoLog);
        Error("[Shader::Compile] Shader program linking failed: " + std::string(infoLog));
    }

    LogOnDebug("[Shader::Compile] Shader program successfully linked");

    // Delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    LogOnDebug("[Shader::Compile] Shader program compiled and linked successfully (ID: " + std::to_string(id) + ")");
}

void Shader::SetUniformFloat(const char* name, float value)
{
    glUniform1f(glGetUniformLocation(id, name), value);
}

void Shader::SetUniformVector2f(const char* name, const float x, const float y)
{
    glUniform2f(glGetUniformLocation(id, name), x, y);
}

void Shader::SetUniformVector2f(const char* name, const glm::vec2& value)
{
    glUniform2f(glGetUniformLocation(id, name), value.x, value.y);
}

void Shader::SetUniformVector3f(const char* name, const float x, const float y, const float z)
{
    glUniform3f(glGetUniformLocation(id, name), x, y, z);
}

void Shader::SetUniformVector3f(const char* name, const glm::vec3& value)
{
    glUniform3f(glGetUniformLocation(id, name), value.x, value.y, value.z);
}

void Shader::SetUniformVector4f(const char* name, const float x, const float y, const float z, const float w)
{
    glUniform4f(glGetUniformLocation(id, name), x, y, z, w);
}

void Shader::SetUniformVector4f(const char* name, const glm::vec4& value)
{
    glUniform4f(glGetUniformLocation(id, name), value.x, value.y, value.z, value.w);
}

void Shader::SetUniformMatrix4f(const char* name, const glm::mat4& matrix)
{
    glUniformMatrix4fv(glGetUniformLocation(id, name), 1, GL_FALSE, glm::value_ptr(matrix));
}
