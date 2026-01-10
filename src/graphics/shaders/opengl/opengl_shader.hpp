#pragma once

#include "../shader.hpp"
#include "glad/glad.h"
#include "glm/glm.hpp"

class OpenGLShader : public IShader
{
public:
    OpenGLShader() = default;
    ~OpenGLShader() override;

    void compile(const std::string& vertexSource, const std::string& fragmentSource) override;
    void use() const override;
    void setUniformInt(const char* name, int value) const override;
    void setUniformFloat(const char* name, float value) const override;
    void setUniformVec2(const char* name, float x, float y) const override;
    void setUniformVec2(const char* name, const glm::vec2& value) const override;
    void setUniformVec3(const char* name, float x, float y, float z) const override;
    void setUniformVec3(const char* name, const glm::vec3& value) const override;
    void setUniformVec4(const char* name, float x, float y, float z, float w) const override;
    void setUniformVec4(const char* name, const glm::vec4& value) const override;
    void setUniformMat3(const char* name, const glm::mat3& matrix) const override;
    void setUniformMat4(const char* name, const glm::mat4& matrix) const override;

private:
    unsigned int id;
};
