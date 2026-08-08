#pragma once

#include "core/project/asset_manager.hpp"

#include <memory>

class Material;
class Mesh;
class Texture;

class AssetDefaults
{
public:
    AssetDefaults(std::shared_ptr<MaterialAssetManager> materialManager, std::shared_ptr<TextureAssetManager> textureManager);
    ~AssetDefaults() = default;

    AssetID getDefaultMaterial();
    std::shared_ptr<Texture> getDefaultTexture();

    Material applyToMaterial(const Material& material);
    Mesh applyToMesh(const Mesh& mesh);

    void reset();

private:
    std::shared_ptr<MaterialAssetManager> materialManager;
    std::shared_ptr<TextureAssetManager> textureManager;
    AssetID defaultMaterialID = INVALID_ASSET_ID;
    std::weak_ptr<Texture> defaultTexture;
};
