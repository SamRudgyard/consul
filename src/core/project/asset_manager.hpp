#pragma once

#include "core/project/asset_types.hpp"

#include <algorithm>
#include <map>
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
        nonOwningAssets.push_back(assetPointer);
        assetMetadata.emplace(assetPointer, AssetMetadata{name, {}});
        return assetPointer;
    }

    std::optional<AssetMetadata> getMetadata(const std::shared_ptr<T>& asset) const
    {
        if (!asset) {
            return std::nullopt;
        }

        auto it = assetMetadata.find(std::weak_ptr<T>(asset));
        if (it == assetMetadata.end()) {
            return std::nullopt;
        }

        return it->second;
    }

    std::vector<std::shared_ptr<T>> getAssets() const
    {
        std::vector<std::shared_ptr<T>> activeAssets;
        activeAssets.reserve(nonOwningAssets.size() + preservedAssets.size());

        for (const std::weak_ptr<T>& nonOwningAsset : nonOwningAssets) {
            std::shared_ptr<T> asset = nonOwningAsset.lock();
            if (!asset) {
                continue;
            }

            activeAssets.push_back(std::move(asset));
        }

        activeAssets.insert(activeAssets.end(), preservedAssets.begin(), preservedAssets.end());

        return activeAssets;
    }

    void removeExpiredAssets()
    {
        auto it = nonOwningAssets.begin();
        while (it != nonOwningAssets.end()) {
            if (it->expired()) {
                assetMetadata.erase(*it);
                it = nonOwningAssets.erase(it);
            } else {
                ++it;
            }
        }
    }

    void setPreserved(const std::shared_ptr<T>& asset, bool preserved)
    {
        if (!asset) {
            return;
        }

        if (preserved) {
            auto it = std::find_if(nonOwningAssets.begin(), nonOwningAssets.end(), [&asset](const std::weak_ptr<T>& candidate) {
                return candidate.lock() == asset;
            });
            if (it == nonOwningAssets.end()) {
                return;
            }

            preservedAssets.push_back(asset);
            nonOwningAssets.erase(it);
            return;
        }

        auto it = std::find(preservedAssets.begin(), preservedAssets.end(), asset);
        if (it == preservedAssets.end()) {
            return;
        }

        nonOwningAssets.push_back(*it);
        preservedAssets.erase(it);
    }

    bool isPreserved(const std::shared_ptr<T>& asset) const
    {
        if (!asset) {
            return false;
        }

        return std::find(preservedAssets.begin(), preservedAssets.end(), asset) != preservedAssets.end();
    }

    void setSourcePath(const std::shared_ptr<T>& asset, const std::string& sourcePath)
    {
        if (!asset) {
            return;
        }

        auto it = assetMetadata.find(std::weak_ptr<T>(asset));
        if (it == assetMetadata.end()) {
            return;
        }

        it->second.sourcePath = sourcePath;
    }

private:
    std::vector<std::weak_ptr<T>> nonOwningAssets;
    std::vector<std::shared_ptr<T>> preservedAssets;
    std::map<std::weak_ptr<T>, AssetMetadata, std::owner_less<>> assetMetadata;
};

using MaterialAssetManager = AssetManager<Material>;
using MeshAssetManager = AssetManager<Mesh>;
using ModelAssetManager = AssetManager<Model>;
using ShaderAssetManager = AssetManager<Shader>;
using FragmentShaderAssetManager = AssetManager<FragmentShader>;
using TextureAssetManager = AssetManager<Texture>;
using VertexShaderAssetManager = AssetManager<VertexShader>;
