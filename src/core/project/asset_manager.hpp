#pragma once

#include "core/project/asset_types.hpp"

#include <memory>
#include <string>
#include <unordered_map>

class Material;
class Mesh;
class Model;
class Shader;
class FragmentShader;
class Texture;
class VertexShader;

template <typename T>
class AssetManager
{
public:
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

    const std::unordered_map<AssetID, std::shared_ptr<T>>& getAssets() const { return assets; }

    void setSourcePath(AssetID id, const std::string& sourcePath)
    {
        auto it = metadata.find(id);
        if (it == metadata.end()) {
            return;
        }

        it->second.sourcePath = sourcePath;
    }

    void clearAssets()
    {
        metadata.clear();
        assets.clear();
    }

private:
    std::unordered_map<AssetID, AssetMetadata> metadata;
    std::unordered_map<AssetID, std::shared_ptr<T>> assets;

    void addMetadata(AssetID id, const std::string& name)
    {
        metadata[id] = AssetMetadata{
            name,
            {}
        };
    }
};

using MaterialAssetManager = AssetManager<Material>;
using MeshAssetManager = AssetManager<Mesh>;
using ModelAssetManager = AssetManager<Model>;
using ShaderAssetManager = AssetManager<Shader>;
using FragmentShaderAssetManager = AssetManager<FragmentShader>;
using TextureAssetManager = AssetManager<Texture>;
using VertexShaderAssetManager = AssetManager<VertexShader>;
