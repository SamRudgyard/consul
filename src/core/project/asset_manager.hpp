#pragma once

#include "core/project/asset_types.hpp"

#include <algorithm>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

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

    std::shared_ptr<T> add(const std::string& name, const T& asset)
    {
        std::shared_ptr<T> assetPointer = std::make_shared<T>(asset);
        assets.push_back({assetPointer, {name, {}}});
        return assetPointer;
    }

    std::optional<AssetMetadata> getMetadata(const std::shared_ptr<T>& asset) const
    {
        if (!asset) {
            return std::nullopt;
        }

        auto it = findAsset(asset);
        if (it == assets.end()) {
            return std::nullopt;
        }

        return it->metadata;
    }

    std::vector<std::shared_ptr<T>> getAssets() const
    {
        std::vector<std::shared_ptr<T>> activeAssets;
        activeAssets.reserve(assets.size());

        auto it = assets.begin();
        while (it != assets.end()) {
            std::shared_ptr<T> asset = it->asset.lock();
            if (asset) {
                activeAssets.push_back(std::move(asset));
                ++it;
            } else {
                it = assets.erase(it);
            }
        }

        return activeAssets;
    }

    void setSourcePath(const std::shared_ptr<T>& asset, const std::string& sourcePath)
    {
        if (!asset) {
            return;
        }

        auto it = findAsset(asset);
        if (it == assets.end()) {
            return;
        }

        it->metadata.sourcePath = sourcePath;
    }

private:
    struct AssetEntry
    {
        std::weak_ptr<T> asset;
        AssetMetadata metadata;
    };

    mutable std::vector<AssetEntry> assets;

    typename std::vector<AssetEntry>::iterator findAsset(const std::shared_ptr<T>& asset) const
    {
        return std::find_if(assets.begin(), assets.end(), [&asset](const AssetEntry& entry) {
            return entry.asset.lock() == asset;
        });
    }
};

using MaterialAssetManager = AssetManager<Material>;
using MeshAssetManager = AssetManager<Mesh>;
using ModelAssetManager = AssetManager<Model>;
using ShaderAssetManager = AssetManager<Shader>;
using FragmentShaderAssetManager = AssetManager<FragmentShader>;
using TextureAssetManager = AssetManager<Texture>;
using VertexShaderAssetManager = AssetManager<VertexShader>;
