#pragma once

#include <string>

class Shader {
public:
    Shader(const std::string& vertexFilePath, const std::string& fragmentFilePath);

    unsigned int getID() const { return id; }
    const std::string& getVertexSource() const { return vertexSource; }
    const std::string& getFragmentSource() const { return fragmentSource; }
private:
    unsigned int id;
    std::string vertexSource;
    std::string fragmentSource;
};
