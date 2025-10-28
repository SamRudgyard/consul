#include "shader.hpp"

#include "core/console/console.hpp"
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
 * @param vertexSource String containing the GLSL source code for the vertex shader.
 * @param fragmentSource String containing the GLSL source code for the fragment shader.
 */
void Shader::Compile(const std::string& vertexSource, const std::string& fragmentSource)
{   
    Console& console = Console::Get();

    // Create shaders
    unsigned int vertex, fragment;
    vertex = glCreateShader(GL_VERTEX_SHADER);
    checkOpenGLErrors("[Shader::Compile] Error creating vertex shader");
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    checkOpenGLErrors("[Shader::Compile] Error creating fragment shader");

    // Compile vertex shader
    const char* vertexCString = vertexSource.c_str();
    glShaderSource(vertex, 1, &vertexCString, NULL);
    checkOpenGLErrors("[Shader::Compile] Error setting vertex shader source");
    glCompileShader(vertex);
    checkOpenGLErrors("[Shader::Compile] Error compiling vertex shader");
    // Check for compilation errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        console.Error("[Shader::Compile] Vertex shader compilation failed: " + std::string(infoLog));
    }

    console.LogOnDebug("[Shader::Compile] Vertex shader successfully compiled");

    // Compile fragment shader
    const char* fragmentCString = fragmentSource.c_str();
    glShaderSource(fragment, 1, &fragmentCString, NULL);
    checkOpenGLErrors("[Shader::Compile] Error setting fragment shader source");
    glCompileShader(fragment);
    checkOpenGLErrors("[Shader::Compile] Error compiling fragment shader");
    // Check for compilation errors
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        console.Error("[Shader::Compile] Fragment shader compilation failed: " + std::string(infoLog));
    }

    console.LogOnDebug("[Shader::Compile] Fragment shader successfully compiled");

    // Create shader program and link shaders
    id = glCreateProgram();
    checkOpenGLErrors("[Shader::Compile] Error creating shader program");
    glAttachShader(id, vertex);
    glAttachShader(id, fragment);
    checkOpenGLErrors("[Shader::Compile] Error attaching shaders to program");
    glLinkProgram(id);
    checkOpenGLErrors("[Shader::Compile] Error linking shader program");
    // Check for linking errors
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(id, 512, NULL, infoLog);
        console.Error("[Shader::Compile] Shader program linking failed: " + std::string(infoLog));
    }

    console.LogOnDebug("[Shader::Compile] Shader program successfully linked");

    // Delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    checkOpenGLErrors("[Shader::Compile] Error deleting shaders");

    console.LogOnDebug("[Shader::Compile] Shader program compiled and linked successfully (ID: " + std::to_string(id) + ")");
}

void Shader::SetUniformInt(const char* name, int value)
{
    glUniform1i(glGetUniformLocation(id, name), value);
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
