#pragma once

#include <cstdint>
#include <cstddef>
#include <functional>
#include <string>

using AssetID = std::uint64_t;

constexpr AssetID INVALID_ASSET_ID = 0;

enum class AssetType
{
    MATERIAL,
    MESH,
    MODEL,
    SHADER,
    TEXTURE
};

struct AssetMetadata
{
    AssetID id = INVALID_ASSET_ID;
    AssetType type;
    std::string name;
    std::string sourcePath;
    std::string secondarySourcePath;
    bool importedFromFile;
};
