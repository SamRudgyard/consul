#pragma once

#include "core/project/asset_manager.hpp"

#include <memory>

class Material;
class Mesh;

class AssetDefaults
{
public:
    AssetDefaults(std::shared_ptr<MaterialAssetManager> materialManager, std::shared_ptr<TextureAssetManager> textureManager);
    ~AssetDefaults() = default;

    AssetID getDefaultMaterial();
    AssetID getDefaultTexture();

    Material applyToMaterial(const Material& material);
    Mesh applyToMesh(const Mesh& mesh);

    void reset();

private:
    std::shared_ptr<MaterialAssetManager> materialManager;
    std::shared_ptr<TextureAssetManager> textureManager;
    AssetID defaultMaterialID = INVALID_ASSET_ID;
    AssetID defaultTextureID = INVALID_ASSET_ID;
};
