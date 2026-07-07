#pragma once

#include "core/project/asset_types.hpp"

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
    Shader(AssetID vertexShaderID, AssetID fragmentShaderID);

    unsigned int getID() const { return id; }
    AssetID getVertexShader() const { return vertexShaderID; }
    AssetID getFragmentShader() const { return fragmentShaderID; }

private:
    unsigned int id;
    AssetID vertexShaderID = INVALID_ASSET_ID;
    AssetID fragmentShaderID = INVALID_ASSET_ID;
};
