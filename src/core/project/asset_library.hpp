#pragma once

#include "core/project/asset_manager.hpp"

#include <memory>
#include <string>

class AssetDefaults;
class GLTFImporter;

class AssetLibrary
{
public:
    AssetLibrary(
        std::shared_ptr<ModelAssetManager> modelManager,
        std::shared_ptr<MeshAssetManager> meshManager,
        std::shared_ptr<MaterialAssetManager> materialManager,
        std::shared_ptr<TextureAssetManager> textureManager,
        std::shared_ptr<ShaderAssetManager> shaderManager,
        std::shared_ptr<AssetDefaults> assetDefaults,
        std::shared_ptr<GLTFImporter> gltfImporter
    );
    ~AssetLibrary() = default;

    AssetID getDefaultMaterial();
    AssetID getDefaultTexture();

    AssetID addModel(const std::string& name, const Model& model);
    AssetID addMesh(const std::string& name, const Mesh& mesh);
    AssetID addTexture(const std::string& name, const Texture& texture);
    AssetID addMaterial(const std::string& name, const Material& material);
    AssetID addShader(const std::string& name, const Shader& shader);

    AssetID importAsset(const std::string& name, const std::string& path);
    AssetID importShader(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath);

    std::shared_ptr<Model> getModel(AssetID id) const;
    std::shared_ptr<Mesh> getMesh(AssetID id) const;
    std::shared_ptr<Texture> getTexture(AssetID id) const;
    std::shared_ptr<Material> getMaterial(AssetID id) const;
    std::shared_ptr<Shader> getShader(AssetID id) const;

    const AssetMetadata* getMetadata(AssetID id) const;
    const ModelAssetManager::AssetMap& getModels() const;
    const MeshAssetManager::AssetMap& getMeshes() const;
    const MaterialAssetManager::AssetMap& getMaterials() const;
    const TextureAssetManager::AssetMap& getTextures() const;
    const ShaderAssetManager::AssetMap& getShaders() const;

    void clearAssets();

private:
    std::shared_ptr<ModelAssetManager> modelManager;
    std::shared_ptr<MeshAssetManager> meshManager;
    std::shared_ptr<MaterialAssetManager> materialManager;
    std::shared_ptr<TextureAssetManager> textureManager;
    std::shared_ptr<ShaderAssetManager> shaderManager;
    std::shared_ptr<AssetDefaults> assetDefaults;
    std::shared_ptr<GLTFImporter> gltfImporter;
};
