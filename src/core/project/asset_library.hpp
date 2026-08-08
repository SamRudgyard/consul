#pragma once

#include "core/project/asset_manager.hpp"

#include <memory>
#include <string>
#include <vector>

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
        std::shared_ptr<VertexShaderAssetManager> vertexShaderManager,
        std::shared_ptr<FragmentShaderAssetManager> fragmentShaderManager,
        std::shared_ptr<AssetDefaults> assetDefaults,
        std::shared_ptr<GLTFImporter> gltfImporter
    );
    ~AssetLibrary() = default;

    std::shared_ptr<Material> getDefaultMaterial();
    std::shared_ptr<Texture> getDefaultTexture();

    std::shared_ptr<Model> addModel(const std::string& name, const Model& model);
    std::shared_ptr<Mesh> addMesh(const std::string& name, const Mesh& mesh);
    std::shared_ptr<Texture> addTexture(const std::string& name, const Texture& texture);
    std::shared_ptr<Material> addMaterial(const std::string& name, const Material& material);
    std::shared_ptr<Shader> addShader(const std::string& name, const Shader& shader);
    std::shared_ptr<VertexShader> addVertexShader(const std::string& name, const VertexShader& shader);
    std::shared_ptr<FragmentShader> addFragmentShader(const std::string& name, const FragmentShader& shader);

    std::shared_ptr<Model> importAsset(const std::string& name, const std::string& path);
    std::shared_ptr<Shader> importShader(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath);

    std::vector<std::shared_ptr<Model>> getModels() const;
    std::vector<std::shared_ptr<Mesh>> getMeshes() const;
    std::vector<std::shared_ptr<Material>> getMaterials() const;
    std::vector<std::shared_ptr<Texture>> getTextures() const;
    std::vector<std::shared_ptr<Shader>> getShaders() const;
    std::vector<std::shared_ptr<VertexShader>> getVertexShaders() const;
    std::vector<std::shared_ptr<FragmentShader>> getFragmentShaders() const;

private:
    std::shared_ptr<ModelAssetManager> modelManager;
    std::shared_ptr<MeshAssetManager> meshManager;
    std::shared_ptr<MaterialAssetManager> materialManager;
    std::shared_ptr<TextureAssetManager> textureManager;
    std::shared_ptr<ShaderAssetManager> shaderManager;
    std::shared_ptr<VertexShaderAssetManager> vertexShaderManager;
    std::shared_ptr<FragmentShaderAssetManager> fragmentShaderManager;
    std::shared_ptr<AssetDefaults> assetDefaults;
    std::shared_ptr<GLTFImporter> gltfImporter;
};
