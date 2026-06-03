#pragma once

#include "core/project/asset_types.hpp"

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

    AssetID addModel(const std::string& name, std::shared_ptr<Model> model);
    AssetID addMesh(const std::string& name, std::shared_ptr<Mesh> mesh);
    AssetID addTexture(const std::string& name, std::shared_ptr<Texture> texture);
    AssetID addMaterial(const std::string& name, std::shared_ptr<Material> material);
    AssetID addShader(const std::string& name, std::shared_ptr<Shader> shader);

    AssetID importAsset(const std::string& name, const std::string& path);
    AssetID importShader(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath);

    std::shared_ptr<Model> getModel(AssetID id) const;
    std::shared_ptr<Mesh> getMesh(AssetID id) const;
    std::shared_ptr<Texture> getTexture(AssetID id) const;
    std::shared_ptr<Material> getMaterial(AssetID id) const;
    std::shared_ptr<Shader> getShader(AssetID id) const;

    const AssetMetadata* getMetadata(AssetID id) const;

private:
    AssetID nextID = 1;
    std::unordered_map<AssetID, AssetMetadata> metadata;
    std::unordered_map<AssetID, std::shared_ptr<Model>> models;
    std::unordered_map<AssetID, std::shared_ptr<Mesh>> meshes;
    std::unordered_map<AssetID, std::shared_ptr<Texture>> textures;
    std::unordered_map<AssetID, std::shared_ptr<Material>> materials;
    std::unordered_map<AssetID, std::shared_ptr<Shader>> shaders;

    static inline const std::vector<std::string> supportedModelExtensions = { ".gltf" };
    static inline const std::vector<std::string> supportedTextureExtensions = { ".png", ".jpg", ".jpeg" };
    static inline const std::vector<std::string> supportedMeshExtensions = {};
    static inline const std::vector<std::string> supportedMaterialExtensions = {};
    static inline const std::vector<std::string> supportedShaderExtensions = {};

    AssetID reserveID();

    void addMetadata(AssetID id, AssetType type, const std::string& name);
    static std::string getLowerExtension(const std::string& path);
};
