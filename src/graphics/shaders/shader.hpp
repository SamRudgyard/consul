#pragma once

#include <string>
#include "glm/glm.hpp"

class IShader // TODO: Rename to Shader, or perhaps RenderableShader if we have a Shader class for the source code?
{
public:
    IShader() = default;
    virtual ~IShader() = default;

    virtual void compile(const std::string& vertexSource, const std::string& fragmentSource) = 0;
    virtual void use() const = 0;
    virtual void setUniformInt (const char* name, int value) const = 0;
    virtual void setUniformFloat(const char* name, float value) const = 0;
    virtual void setUniformVec2(const char* name, float x, float y) const = 0;
    virtual void setUniformVec2(const char* name, const glm::vec2& value) const = 0;
    virtual void setUniformVec3(const char* name, float x, float y, float z) const = 0;
    virtual void setUniformVec3(const char* name, const glm::vec3& value) const = 0;
    virtual void setUniformVec4(const char* name, float x, float y, float z, float w) const = 0;
    virtual void setUniformVec4(const char* name, const glm::vec4& value) const = 0;
    virtual void setUniformMat3(const char* name, const glm::mat3& matrix) const = 0;
    virtual void setUniformMat4(const char* name, const glm::mat4& matrix) const = 0;
};
