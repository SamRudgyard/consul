#pragma once

#include "core/project/asset_types.hpp"
#include "core/project/importers/gltf_importer.hpp"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class Material;
class Mesh;
class Model;
class Shader;
class Texture;

class AssetManager
{
public:
    AssetManager() = default;
    ~AssetManager() = default;

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
    const std::unordered_map<AssetID, std::shared_ptr<Model>>& getModels() const { return models; }
    const std::unordered_map<AssetID, std::shared_ptr<Mesh>>& getMeshes() const { return meshes; }
    const std::unordered_map<AssetID, std::shared_ptr<Shader>>& getShaders() const { return shaders; }

    void clearAssets();

private:
    std::unordered_map<AssetID, AssetMetadata> metadata;
    std::unordered_map<AssetID, std::shared_ptr<Model>> models;
    std::unordered_map<AssetID, std::shared_ptr<Mesh>> meshes;
    std::unordered_map<AssetID, std::shared_ptr<Texture>> textures;
    std::unordered_map<AssetID, std::shared_ptr<Material>> materials;
    std::unordered_map<AssetID, std::shared_ptr<Shader>> shaders;

    GLTFImporter gltfImporter;

    void addMetadata(AssetID id, AssetType type, const std::string& name);
    AssetID loadModelFromGLTF(const std::string& name, const std::string& path);
};
