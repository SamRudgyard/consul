#pragma once

#include <string>
#include "glm/glm.hpp"

class IShader
{
public:
    IShader() = default;
    virtual ~IShader() = default;

    virtual void compile(const std::string& vertexSource, const std::string& fragmentSource) = 0;
    virtual void use() = 0;
    virtual void setUniformInt(const char* name, int value) = 0;
    virtual void setUniformFloat(const char* name, float value) = 0;
    virtual void setUniformVec2(const char* name, float x, float y) = 0;
    virtual void setUniformVec2(const char* name, const glm::vec2& value) = 0;
    virtual void setUniformVec3(const char* name, float x, float y, float z) = 0;
    virtual void setUniformVec3(const char* name, const glm::vec3& value) = 0;
    virtual void setUniformVec4(const char* name, float x, float y, float z, float w) = 0;
    virtual void setUniformVec4(const char* name, const glm::vec4& value) = 0;
    virtual void setUniformMat4(const char* name, const glm::mat4& matrix) = 0;
};