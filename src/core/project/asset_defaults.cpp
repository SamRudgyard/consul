#include "core/project/asset_defaults.hpp"

#include "graphics/material/material.hpp"
#include "graphics/mesh/mesh.hpp"
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
        AssetID materialID = materialManager->add("Default Material", material);
        materialAsset = materialManager->get(materialID);
        defaultMaterial = materialAsset;
    }

    return materialAsset;
}

std::shared_ptr<Texture> AssetDefaults::getDefaultTexture()
{
    std::shared_ptr<Texture> texture = defaultTexture.lock();
    if (!texture) {
        AssetID textureID = textureManager->add("Default Texture", Texture());
        texture = textureManager->get(textureID);
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

Mesh AssetDefaults::applyToMesh(const Mesh& mesh)
{
    Mesh meshWithDefaults = mesh;
    if (!meshWithDefaults.getMaterial()) {
        meshWithDefaults.setMaterial(getDefaultMaterial());
    }

    return meshWithDefaults;
}

void AssetDefaults::reset()
{
    defaultMaterial.reset();
    defaultTexture.reset();
}
