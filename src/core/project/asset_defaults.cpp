#include "core/project/asset_defaults.hpp"

#include "graphics/material/material.hpp"
#include "graphics/texture/texture.hpp"

AssetDefaults::AssetDefaults(std::shared_ptr<MaterialAssetManager> materialManager, std::shared_ptr<TextureAssetManager> textureManager)
    : materialManager(materialManager),
      textureManager(textureManager)
{
}

std::shared_ptr<Material> AssetDefaults::getDefaultMaterial()
{
    std::shared_ptr<Material> materialAsset = defaultMaterial.lock();
    if (!materialAsset) {
        Material material;
        material.setAlbedoTexture(getDefaultTexture());
        material.setSpecularTexture(getDefaultTexture());
        materialAsset = materialManager->add("Default Material", material);
        defaultMaterial = materialAsset;
    }

    return materialAsset;
}

std::shared_ptr<Texture> AssetDefaults::getDefaultTexture()
{
    std::shared_ptr<Texture> texture = defaultTexture.lock();
    if (!texture) {
        texture = textureManager->add("Default Texture", Texture());
        defaultTexture = texture;
    }

    return texture;
}

Material AssetDefaults::applyToMaterial(const Material& material)
{
    Material materialWithDefaults = material;
    if (!materialWithDefaults.getAlbedoTexture()) {
        materialWithDefaults.setAlbedoTexture(getDefaultTexture());
    }
    if (!materialWithDefaults.getSpecularTexture()) {
        materialWithDefaults.setSpecularTexture(getDefaultTexture());
    }

    return materialWithDefaults;
}
