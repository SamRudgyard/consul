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
    return modelManager->add(name, model);
}

std::shared_ptr<Mesh> AssetLibrary::addMesh(const std::string& name, const Mesh& mesh)
{
    return meshManager->add(name, mesh);
}

std::shared_ptr<Texture> AssetLibrary::addTexture(const std::string& name, const Texture& texture)
{
    return textureManager->add(name, texture);
}

std::shared_ptr<Material> AssetLibrary::addMaterial(const std::string& name, const Material& material)
{
    return materialManager->add(name, assetDefaults->applyToMaterial(material));
}

std::shared_ptr<Shader> AssetLibrary::addShader(const std::string& name, const Shader& shader)
{
    return shaderManager->add(name, shader);
}

std::shared_ptr<VertexShader> AssetLibrary::addVertexShader(const std::string& name, const VertexShader& shader)
{
    return vertexShaderManager->add(name, shader);
}

std::shared_ptr<FragmentShader> AssetLibrary::addFragmentShader(const std::string& name, const FragmentShader& shader)
{
    return fragmentShaderManager->add(name, shader);
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

    std::shared_ptr<VertexShader> vertexShader = vertexShaderManager->add(name + "_VertexShader", VertexShader(readFile(vertexPath.c_str())));
    vertexShaderManager->setSourcePath(vertexShader, vertexPath);

    std::shared_ptr<FragmentShader> fragmentShader = fragmentShaderManager->add(name + "_FragmentShader", FragmentShader(readFile(fragmentPath.c_str())));
    fragmentShaderManager->setSourcePath(fragmentShader, fragmentPath);

    return addShader(name, Shader(std::move(vertexShader), std::move(fragmentShader)));
}

std::vector<std::shared_ptr<Model>> AssetLibrary::getModels() const
{
    return modelManager->getAssets();
}

std::vector<std::shared_ptr<Mesh>> AssetLibrary::getMeshes() const
{
    return meshManager->getAssets();
}

std::vector<std::shared_ptr<Material>> AssetLibrary::getMaterials() const
{
    return materialManager->getAssets();
}

std::vector<std::shared_ptr<Texture>> AssetLibrary::getTextures() const
{
    return textureManager->getAssets();
}

std::vector<std::shared_ptr<Shader>> AssetLibrary::getShaders() const
{
    return shaderManager->getAssets();
}

std::vector<std::shared_ptr<VertexShader>> AssetLibrary::getVertexShaders() const
{
    return vertexShaderManager->getAssets();
}

std::vector<std::shared_ptr<FragmentShader>> AssetLibrary::getFragmentShaders() const
{
    return fragmentShaderManager->getAssets();
}
