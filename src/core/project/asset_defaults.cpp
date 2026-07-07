#include "core/project/asset_defaults.hpp"

#include "graphics/material/material.hpp"
#include "graphics/mesh/mesh.hpp"
#include "graphics/texture/texture.hpp"

AssetDefaults::AssetDefaults(std::shared_ptr<MaterialAssetManager> materialManager, std::shared_ptr<TextureAssetManager> textureManager)
    : materialManager(materialManager),
      textureManager(textureManager)
{
}

AssetID AssetDefaults::getDefaultMaterial()
{
    if (defaultMaterialID == INVALID_ASSET_ID || !materialManager->get(defaultMaterialID)) {
        Material material;
        material.setAlbedoTextureID(getDefaultTexture());
        material.setSpecularTextureID(getDefaultTexture());
        defaultMaterialID = materialManager->add("Default Material", material);
    }

    return defaultMaterialID;
}

AssetID AssetDefaults::getDefaultTexture()
{
    if (defaultTextureID == INVALID_ASSET_ID || !textureManager->get(defaultTextureID)) {
        defaultTextureID = textureManager->add("Default Texture", Texture());
    }

    return defaultTextureID;
}

Material AssetDefaults::applyToMaterial(const Material& material)
{
    Material materialWithDefaults = material;
    if (materialWithDefaults.getAlbedoTextureID() == INVALID_ASSET_ID) {
        materialWithDefaults.setAlbedoTextureID(getDefaultTexture());
    }
    if (materialWithDefaults.getSpecularTextureID() == INVALID_ASSET_ID) {
        materialWithDefaults.setSpecularTextureID(getDefaultTexture());
    }

    return materialWithDefaults;
}

Mesh AssetDefaults::applyToMesh(const Mesh& mesh)
{
    Mesh meshWithDefaults = mesh;
    if (meshWithDefaults.getMaterial() == INVALID_ASSET_ID) {
        meshWithDefaults.setMaterial(getDefaultMaterial());
    }

    return meshWithDefaults;
}

void AssetDefaults::reset()
{
    defaultMaterialID = INVALID_ASSET_ID;
    defaultTextureID = INVALID_ASSET_ID;
}
