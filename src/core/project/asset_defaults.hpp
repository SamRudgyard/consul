#pragma once

#include "core/project/asset_manager.hpp"

#include <memory>

class Material;
class Texture;

class AssetDefaults
{
public:
    AssetDefaults(std::shared_ptr<MaterialAssetManager> materialManager, std::shared_ptr<TextureAssetManager> textureManager);
    ~AssetDefaults() = default;

    std::shared_ptr<Material> getDefaultMaterial();
    std::shared_ptr<Texture> getDefaultTexture();

    Material applyToMaterial(const Material& material);

private:
    std::shared_ptr<MaterialAssetManager> materialManager;
    std::shared_ptr<TextureAssetManager> textureManager;
    std::weak_ptr<Material> defaultMaterial;
    std::weak_ptr<Texture> defaultTexture;
};
