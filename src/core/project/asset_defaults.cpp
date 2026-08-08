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
        material.setAlbedoTexture(getDefaultTexture());
        material.setSpecularTexture(getDefaultTexture());
        defaultMaterialID = materialManager->add("Default Material", material);
    }

    return defaultMaterialID;
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
    if (meshWithDefaults.getMaterial() == INVALID_ASSET_ID) {
        meshWithDefaults.setMaterial(getDefaultMaterial());
    }

    return meshWithDefaults;
}

void AssetDefaults::reset()
{
    defaultMaterialID = INVALID_ASSET_ID;
    defaultTexture.reset();
}
