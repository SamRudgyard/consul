#include "core/project/asset_library.hpp"

#include "core/console/console.hpp"
#include "core/project/asset_defaults.hpp"
#include "core/project/importers/gltf_importer.hpp"
#include "graphics/material/material.hpp"
#include "graphics/mesh/mesh.hpp"
#include "graphics/models/model.hpp"
#include "graphics/shader/shader.hpp"
#include "graphics/texture/texture.hpp"
#include "utils.hpp"

#include <utility>

AssetLibrary::AssetLibrary(
    std::shared_ptr<ModelAssetManager> modelManager,
    std::shared_ptr<MeshAssetManager> meshManager,
    std::shared_ptr<MaterialAssetManager> materialManager,
    std::shared_ptr<TextureAssetManager> textureManager,
    std::shared_ptr<ShaderAssetManager> shaderManager,
    std::shared_ptr<VertexShaderAssetManager> vertexShaderManager,
    std::shared_ptr<FragmentShaderAssetManager> fragmentShaderManager,
    std::shared_ptr<AssetDefaults> assetDefaults,
    std::shared_ptr<GLTFImporter> gltfImporter
) : modelManager(modelManager),
    meshManager(meshManager),
    materialManager(materialManager),
    textureManager(textureManager),
    shaderManager(shaderManager),
    vertexShaderManager(vertexShaderManager),
    fragmentShaderManager(fragmentShaderManager),
    assetDefaults(assetDefaults),
    gltfImporter(gltfImporter)
{
}

std::shared_ptr<Material> AssetLibrary::getDefaultMaterial()
{
    return assetDefaults->getDefaultMaterial();
}

std::shared_ptr<Texture> AssetLibrary::getDefaultTexture()
{
    return assetDefaults->getDefaultTexture();
}

std::shared_ptr<Model> AssetLibrary::addModel(const std::string& name, const Model& model)
{
    AssetID modelID = modelManager->add(name, model);
    return modelManager->get(modelID);
}

std::shared_ptr<Mesh> AssetLibrary::addMesh(const std::string& name, const Mesh& mesh)
{
    AssetID meshID = meshManager->add(name, assetDefaults->applyToMesh(mesh));
    return meshManager->get(meshID);
}

AssetID AssetLibrary::addTexture(const std::string& name, const Texture& texture)
{
    return textureManager->add(name, texture);
}

std::shared_ptr<Material> AssetLibrary::addMaterial(const std::string& name, const Material& material)
{
    AssetID materialID = materialManager->add(name, assetDefaults->applyToMaterial(material));
    return materialManager->get(materialID);
}

std::shared_ptr<Shader> AssetLibrary::addShader(const std::string& name, const Shader& shader)
{
    AssetID shaderID = shaderManager->add(name, shader);
    return shaderManager->get(shaderID);
}

std::shared_ptr<VertexShader> AssetLibrary::addVertexShader(const std::string& name, const VertexShader& shader)
{
    AssetID shaderID = vertexShaderManager->add(name, shader);
    return vertexShaderManager->get(shaderID);
}

std::shared_ptr<FragmentShader> AssetLibrary::addFragmentShader(const std::string& name, const FragmentShader& shader)
{
    AssetID shaderID = fragmentShaderManager->add(name, shader);
    return fragmentShaderManager->get(shaderID);
}

std::shared_ptr<Model> AssetLibrary::importAsset(const std::string& name, const std::string& path)
{
    if (!doesFileExist(path.c_str())) {
        Console::get().warn("[AssetLibrary::importAsset] Invalid asset path: '" + path + "'");
        return nullptr;
    }

    const std::string extension = getFileExtension(path.c_str());
    if (extension != ".gltf") {
        Console::get().warn("[AssetLibrary::importAsset] Unsupported asset extension: '" + extension + "'");
        return nullptr;
    }

    return gltfImporter->import(name, path);
}

std::shared_ptr<Shader> AssetLibrary::importShader(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath)
{
    if (!doesFileExist(vertexPath.c_str())) {
        Console::get().warn("[AssetLibrary::importShader] Invalid vertex shader path: '" + vertexPath + "'");
        return nullptr;
    }
    if (!doesFileExist(fragmentPath.c_str())) {
        Console::get().warn("[AssetLibrary::importShader] Invalid fragment shader path: '" + fragmentPath + "'");
        return nullptr;
    }

    AssetID vertexShaderID = vertexShaderManager->add(name + "_VertexShader", VertexShader(readFile(vertexPath.c_str())));
    vertexShaderManager->setSourcePath(vertexShaderID, vertexPath);
    std::shared_ptr<VertexShader> vertexShader = vertexShaderManager->get(vertexShaderID);

    AssetID fragmentShaderID = fragmentShaderManager->add(name + "_FragmentShader", FragmentShader(readFile(fragmentPath.c_str())));
    fragmentShaderManager->setSourcePath(fragmentShaderID, fragmentPath);
    std::shared_ptr<FragmentShader> fragmentShader = fragmentShaderManager->get(fragmentShaderID);

    return addShader(name, Shader(std::move(vertexShader), std::move(fragmentShader)));
}

std::shared_ptr<Texture> AssetLibrary::getTexture(AssetID id) const
{
    return textureManager->get(id);
}

const AssetMetadata* AssetLibrary::getMetadata(AssetID id) const
{
    if (const AssetMetadata* metadata = modelManager->getMetadata(id)) {
        return metadata;
    }
    if (const AssetMetadata* metadata = meshManager->getMetadata(id)) {
        return metadata;
    }
    if (const AssetMetadata* metadata = textureManager->getMetadata(id)) {
        return metadata;
    }
    if (const AssetMetadata* metadata = materialManager->getMetadata(id)) {
        return metadata;
    }
    if (const AssetMetadata* metadata = shaderManager->getMetadata(id)) {
        return metadata;
    }
    if (const AssetMetadata* metadata = vertexShaderManager->getMetadata(id)) {
        return metadata;
    }
    return fragmentShaderManager->getMetadata(id);
}

const std::unordered_map<AssetID, std::shared_ptr<Model>>& AssetLibrary::getModels() const
{
    return modelManager->getAssets();
}

const std::unordered_map<AssetID, std::shared_ptr<Mesh>>& AssetLibrary::getMeshes() const
{
    return meshManager->getAssets();
}

const std::unordered_map<AssetID, std::shared_ptr<Material>>& AssetLibrary::getMaterials() const
{
    return materialManager->getAssets();
}

const std::unordered_map<AssetID, std::shared_ptr<Texture>>& AssetLibrary::getTextures() const
{
    return textureManager->getAssets();
}

const std::unordered_map<AssetID, std::shared_ptr<Shader>>& AssetLibrary::getShaders() const
{
    return shaderManager->getAssets();
}

const std::unordered_map<AssetID, std::shared_ptr<VertexShader>>& AssetLibrary::getVertexShaders() const
{
    return vertexShaderManager->getAssets();
}

const std::unordered_map<AssetID, std::shared_ptr<FragmentShader>>& AssetLibrary::getFragmentShaders() const
{
    return fragmentShaderManager->getAssets();
}

void AssetLibrary::clearAssets()
{
    modelManager->clearAssets();
    meshManager->clearAssets();
    textureManager->clearAssets();
    materialManager->clearAssets();
    shaderManager->clearAssets();
    vertexShaderManager->clearAssets();
    fragmentShaderManager->clearAssets();
    assetDefaults->reset();
}
