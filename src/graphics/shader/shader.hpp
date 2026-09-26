#pragma once

#include <memory>
#include <string>

class VertexShader
{
public:
    VertexShader();
    VertexShader(const std::string& source);

    const std::string& getSource() const { return source; }

private:
    std::string source;

    static std::string getDefaultVertexShaderSource();
};

class FragmentShader
{
public:
    FragmentShader();
    FragmentShader(const std::string& source);

    const std::string& getSource() const { return source; }

private:
    std::string source;

    static std::string getDefaultFragmentShaderSource();
};

class Shader
{
public:
    Shader() = default;
    Shader(const std::shared_ptr<VertexShader>& vertexShader, const std::shared_ptr<FragmentShader>& fragmentShader);

    std::shared_ptr<VertexShader> getVertexShader() const { return vertexShader; }
    std::shared_ptr<FragmentShader> getFragmentShader() const { return fragmentShader; }

private:
    std::shared_ptr<VertexShader> vertexShader = std::make_shared<VertexShader>();
    std::shared_ptr<FragmentShader> fragmentShader = std::make_shared<FragmentShader>();
};
