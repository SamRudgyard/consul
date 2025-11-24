#include "shader.hpp"

#include "core/console/console.hpp"
#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "utils.hpp"

Shader& Shader::use()
{
    glUseProgram(id);
    return *this;
}

void Shader::compile(const std::string& vertexSource, const std::string& fragmentSource)
{   
    Console& console = Console::get();

    // Create shaders
    unsigned int vertex, fragment;
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glCheckError();
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glCheckError();

    // Compile vertex shader
    const char* vertexCString = vertexSource.c_str();
    glShaderSource(vertex, 1, &vertexCString, NULL);
    glCheckError();
    glCompileShader(vertex);
    glCheckError();
    // Check for compilation errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        console.error("[Shader::compile] Vertex shader compilation failed: " + std::string(infoLog));
    }

    console.logOnDebug("[Shader::compile] Vertex shader successfully compiled");

    // Compile fragment shader
    const char* fragmentCString = fragmentSource.c_str();
    glShaderSource(fragment, 1, &fragmentCString, NULL);
    glCheckError();
    glCompileShader(fragment);
    glCheckError();
    // Check for compilation errors
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        console.error("[Shader::compile] Fragment shader compilation failed: " + std::string(infoLog));
    }

    console.logOnDebug("[Shader::compile] Fragment shader successfully compiled");

    // Create shader program and link shaders
    id = glCreateProgram();
    glCheckError();
    glAttachShader(id, vertex);
    glAttachShader(id, fragment);
    glCheckError();
    glLinkProgram(id);
    glCheckError();
    // Check for linking errors
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(id, 512, NULL, infoLog);
        console.error("[Shader::compile] Shader program linking failed: " + std::string(infoLog));
    }

    console.logOnDebug("[Shader::compile] Shader program successfully linked");

    // Delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    glCheckError();

    console.logOnDebug("[Shader::compile] Shader program compiled and linked successfully (ID: " + std::to_string(id) + ")");
}

void Shader::setUniformInt(const char* name, int value)
{
    glUniform1i(glGetUniformLocation(id, name), value);
}

void Shader::setUniformFloat(const char* name, float value)
{
    glUniform1f(glGetUniformLocation(id, name), value);
}

void Shader::setUniformVector2f(const char* name, const float x, const float y)
{
    glUniform2f(glGetUniformLocation(id, name), x, y);
}

void Shader::setUniformVector2f(const char* name, const glm::vec2& value)
{
    glUniform2f(glGetUniformLocation(id, name), value.x, value.y);
}

void Shader::setUniformVector3f(const char* name, const float x, const float y, const float z)
{
    glUniform3f(glGetUniformLocation(id, name), x, y, z);
}

void Shader::setUniformVector3f(const char* name, const glm::vec3& value)
{
    glUniform3f(glGetUniformLocation(id, name), value.x, value.y, value.z);
}

void Shader::setUniformVector4f(const char* name, const float x, const float y, const float z, const float w)
{
    glUniform4f(glGetUniformLocation(id, name), x, y, z, w);
}

void Shader::setUniformVector4f(const char* name, const glm::vec4& value)
{
    glUniform4f(glGetUniformLocation(id, name), value.x, value.y, value.z, value.w);
}

void Shader::setUniformMatrix4f(const char* name, const glm::mat4& matrix)
{
    glUniformMatrix4fv(glGetUniformLocation(id, name), 1, GL_FALSE, glm::value_ptr(matrix));
}
