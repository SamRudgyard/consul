#pragma once

#include "glm/glm.hpp"

class Shader
{
public:
    unsigned int id;

    Shader& Use();

    void Compile(const char* vertexSource, const char* fragmentSource);

    void SetUniformFloat(const char* name, float value);
    void SetUniformVector2f(const char* name, const float x, const float y);
    void SetUniformVector2f(const char* name, const glm::vec2& value);
    void SetUniformVector3f(const char* name, const float x, const float y, const float z);
    void SetUniformVector3f(const char* name, const glm::vec3& value);
    void SetUniformVector4f(const char* name, const float x, const float y, const float z, const float w);
    void SetUniformVector4f(const char* name, const glm::vec4& value);
    void SetUniformMatrix4f(const char* name, const glm::mat4& matrix);
};