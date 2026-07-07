#pragma once

#include "core/project/asset_types.hpp"

#include <memory>
#include <string>
#include <unordered_map>

class Material;
class Mesh;
class Model;
class Shader;
class Texture;

template <typename T>
struct AssetTypeFor;

template <>
struct AssetTypeFor<Material>
{
    static constexpr AssetType value = AssetType::MATERIAL;
};

template <>
struct AssetTypeFor<Mesh>
{
    static constexpr AssetType value = AssetType::MESH;
};

template <>
struct AssetTypeFor<Model>
{
    static constexpr AssetType value = AssetType::MODEL;
};

template <>
struct AssetTypeFor<Shader>
{
    static constexpr AssetType value = AssetType::SHADER;
};

template <>
struct AssetTypeFor<Texture>
{
    static constexpr AssetType value = AssetType::TEXTURE;
};

template <typename T>
class AssetManager
{
public:
    using AssetMap = std::unordered_map<AssetID, std::shared_ptr<T>>;

    AssetManager() = default;
    ~AssetManager() = default;

    AssetID add(const std::string& name, const T& asset)
    {
        AssetID id;
        assets[id] = std::make_shared<T>(asset);
        addMetadata(id, name);
        return id;
    }

    std::shared_ptr<T> get(AssetID id) const
    {
        auto it = assets.find(id);
        return it == assets.end() ? nullptr : it->second;
    }

    const AssetMetadata* getMetadata(AssetID id) const
    {
        auto it = metadata.find(id);
        return it == metadata.end() ? nullptr : &it->second;
    }

    const AssetMap& getAssets() const { return assets; }

    void markImported(AssetID id, const std::string& sourcePath, const std::string& secondarySourcePath = "")
    {
        auto it = metadata.find(id);
        if (it == metadata.end()) {
            return;
        }

        it->second.sourcePath = sourcePath;
        it->second.secondarySourcePath = secondarySourcePath;
        it->second.importedFromFile = true;
    }

    void clearAssets()
    {
        metadata.clear();
        assets.clear();
    }

private:
    std::unordered_map<AssetID, AssetMetadata> metadata;
    AssetMap assets;

    void addMetadata(AssetID id, const std::string& name)
    {
        metadata[id] = AssetMetadata{
            id,
            AssetTypeFor<T>::value,
            name,
            {},
            {},
            false
        };
    }
};

using MaterialAssetManager = AssetManager<Material>;
using MeshAssetManager = AssetManager<Mesh>;
using ModelAssetManager = AssetManager<Model>;
using ShaderAssetManager = AssetManager<Shader>;
using TextureAssetManager = AssetManager<Texture>;
