#pragma once

#include <string>

class Shader {
public:
    Shader(const std::string& vertexFilePath, const std::string& fragmentFilePath);
private:
    unsigned int id;
    std::string vertexSource;
    std::string fragmentSource;
};