#pragma once

#include <memory>
#include <string>

class VertexShader
{
public:
    explicit VertexShader(std::string source);

    const std::string& getSource() const { return source; }

private:
    std::string source;
};

class FragmentShader
{
public:
    explicit FragmentShader(std::string source);

    const std::string& getSource() const { return source; }

private:
    std::string source;
};

class Shader
{
public:
    Shader(
        std::shared_ptr<VertexShader> vertexShader,
        std::shared_ptr<FragmentShader> fragmentShader
    );

    std::shared_ptr<VertexShader> getVertexShader() const { return vertexShader; }
    std::shared_ptr<FragmentShader> getFragmentShader() const { return fragmentShader; }

private:
    std::shared_ptr<VertexShader> vertexShader;
    std::shared_ptr<FragmentShader> fragmentShader;
};
