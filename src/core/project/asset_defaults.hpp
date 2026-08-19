#pragma once

#include "core/project/asset_manager.hpp"

#include <memory>

class Material;
class Texture;

class AssetDefaults
{
public:
    AssetDefaults(MaterialAssetManager& materialManager, TextureAssetManager& textureManager);
    ~AssetDefaults() = default;

    std::shared_ptr<Material> getDefaultMaterial();
    std::shared_ptr<Texture> getDefaultTexture();

    Material applyToMaterial(const Material& material);

private:
    MaterialAssetManager& materialManager;
    TextureAssetManager& textureManager;
    std::weak_ptr<Material> defaultMaterial;
    std::weak_ptr<Texture> defaultTexture;
};
