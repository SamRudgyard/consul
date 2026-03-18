#include "opengl_shader.hpp"

#include "core/console/console.hpp"
#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "utils.hpp"

OpenGLShader::~OpenGLShader()
{
    glDeleteProgram(id);
    glCheckError();
}

void OpenGLShader::use() const
{
    glUseProgram(id);
}

void OpenGLShader::compile(const std::string& vertexSource, const std::string& fragmentSource)
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
        console.error("[OpenGLShader::compile] Vertex shader compilation failed: " + std::string(infoLog));
    }

    console.logOnDebug("[OpenGLShader::compile] Vertex shader successfully compiled");

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
        console.error("[OpenGLShader::compile] Fragment shader compilation failed: " + std::string(infoLog));
    }

    console.logOnDebug("[OpenGLShader::compile] Fragment shader successfully compiled");

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

void OpenGLShader::setUniformInt(const char* name, int value) const
{
    glUniform1i(glGetUniformLocation(id, name), value);
}

void OpenGLShader::setUniformFloat(const char* name, float value) const
{
    glUniform1f(glGetUniformLocation(id, name), value);
}

void OpenGLShader::setUniformVec2(const char* name, const float x, const float y) const
{
    glUniform2f(glGetUniformLocation(id, name), x, y);
}

void OpenGLShader::setUniformVec2(const char* name, const glm::vec2& value) const
{
    glUniform2f(glGetUniformLocation(id, name), value.x, value.y);
}

void OpenGLShader::setUniformVec3(const char* name, const float x, const float y, const float z) const
{
    glUniform3f(glGetUniformLocation(id, name), x, y, z);
}

void OpenGLShader::setUniformVec3(const char* name, const glm::vec3& value) const
{
    glUniform3f(glGetUniformLocation(id, name), value.x, value.y, value.z);
}

void OpenGLShader::setUniformVec4(const char* name, const float x, const float y, const float z, const float w) const
{
    glUniform4f(glGetUniformLocation(id, name), x, y, z, w);
}

void OpenGLShader::setUniformVec4(const char* name, const glm::vec4& value) const
{
    glUniform4f(glGetUniformLocation(id, name), value.x, value.y, value.z, value.w);
}

void OpenGLShader::setUniformMat3(const char* name, const glm::mat3& matrix) const
{
    glUniformMatrix3fv(glGetUniformLocation(id, name), 1, GL_FALSE, glm::value_ptr(matrix));
}

void OpenGLShader::setUniformMat4(const char* name, const glm::mat4& matrix) const
{
    glUniformMatrix4fv(glGetUniformLocation(id, name), 1, GL_FALSE, glm::value_ptr(matrix));
}
