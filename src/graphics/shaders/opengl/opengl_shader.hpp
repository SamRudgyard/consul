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
    void use() override;
    void setUniformInt(const char* name, int value) override;
    void setUniformFloat(const char* name, float value) override;
    void setUniformVec2(const char* name, float x, float y) override;
    void setUniformVec2(const char* name, const glm::vec2& value) override;
    void setUniformVec3(const char* name, float x, float y, float z) override;
    void setUniformVec3(const char* name, const glm::vec3& value) override;
    void setUniformVec4(const char* name, float x, float y, float z, float w) override;
    void setUniformVec4(const char* name, const glm::vec4& value) override;
    void setUniformMat4(const char* name, const glm::mat4& matrix) override;

private:
    unsigned int id;
};