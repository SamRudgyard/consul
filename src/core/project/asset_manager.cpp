#include "core/project/asset_manager.hpp"

#include <algorithm>
#include <cctype>
#include <utility>

#include "core/console/console.hpp"
#include "graphics/material/material.hpp"
#include "graphics/mesh/mesh.hpp"
#include "graphics/models/model.hpp"
#include "graphics/shader/shader.hpp"
#include "graphics/texture/texture.hpp"
#include "utils.hpp"

AssetID AssetManager::addModel(const std::string& name, std::shared_ptr<Model> model)
{
    if (!model) {
        Console::get().warn("[AssetManager::addModel] Cannot add null model asset: '" + name + "'");
        return INVALID_ASSET_ID;
    }

    AssetID id;
    models[id] = std::move(model);
    addMetadata(id, AssetType::MODEL, name);
    return id;
}

AssetID AssetManager::addMesh(const std::string& name, std::shared_ptr<Mesh> mesh)
{
    if (!mesh) {
        Console::get().warn("[AssetManager::addMesh] Cannot add null mesh asset: '" + name + "'");
        return INVALID_ASSET_ID;
    }

    AssetID id;
    meshes[id] = std::move(mesh);
    addMetadata(id, AssetType::MESH, name);
    return id;
}

AssetID AssetManager::addTexture(const std::string& name, std::shared_ptr<Texture> texture)
{
    if (!texture) {
        Console::get().warn("[AssetManager::addTexture] Cannot add null texture asset: '" + name + "'");
        return INVALID_ASSET_ID;
    }

    AssetID id;
    textures[id] = std::move(texture);
    addMetadata(id, AssetType::TEXTURE, name);
    return id;
}

AssetID AssetManager::addMaterial(const std::string& name, std::shared_ptr<Material> material)
{
    if (!material) {
        Console::get().warn("[AssetManager::addMaterial] Cannot add null material asset: '" + name + "'");
        return INVALID_ASSET_ID;
    }

    AssetID id;
    materials[id] = std::move(material);
    addMetadata(id, AssetType::MATERIAL, name);
    return id;
}

AssetID AssetManager::addShader(const std::string& name, std::shared_ptr<Shader> shader)
{
    if (!shader) {
        Console::get().warn("[AssetManager::addShader] Cannot add null shader asset: '" + name + "'");
        return INVALID_ASSET_ID;
    }

    AssetID id;
    shaders[id] = std::move(shader);
    addMetadata(id, AssetType::SHADER, name);
    return id;
}

AssetID AssetManager::importAsset(const std::string& name, const std::string& path)
{
    AssetID id = INVALID_ASSET_ID;

    if (!doesFileExist(path.c_str())) {
        Console::get().warn("[AssetManager::importAsset] Invalid asset path: '" + path + "'");
        return id;
    }

    const std::string extension = getFileExtension(path.c_str());
    if (contains(supportedModelExtensions, extension)) {
        id = addModel(name, std::make_shared<Model>(path.c_str()));
    } else if (contains(supportedTextureExtensions, extension)) {
        id = addTexture(name, std::make_shared<Texture>(path.c_str(), TextureType::DIFFUSE));
    } else {
        Console::get().warn("[AssetManager::importAsset] Unsupported asset extension: '" + extension + "'");
        return INVALID_ASSET_ID;
    }

    metadata[id].sourcePath = path;
    metadata[id].importedFromFile = true;
    return id;
}

AssetID AssetManager::importShader(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath)
{
    AssetID id = INVALID_ASSET_ID;

    if (!doesFileExist(vertexPath.c_str())) {
        Console::get().warn("[AssetManager::importShader] Invalid vertex shader path: '" + vertexPath + "'");
        return INVALID_ASSET_ID;
    }
    if (!doesFileExist(fragmentPath.c_str())) {
        Console::get().warn("[AssetManager::importShader] Invalid fragment shader path: '" + fragmentPath + "'");
        return INVALID_ASSET_ID;
    }

    id = addShader(name, std::make_shared<Shader>(vertexPath.c_str(), fragmentPath.c_str()));
    metadata[id].sourcePath = vertexPath;
    metadata[id].secondarySourcePath = fragmentPath;
    metadata[id].importedFromFile = true;
    return id;
}

std::shared_ptr<Model> AssetManager::getModel(AssetID id) const
{
    auto it = models.find(id);
    return it == models.end() ? nullptr : it->second;
}

std::shared_ptr<Mesh> AssetManager::getMesh(AssetID id) const
{
    auto it = meshes.find(id);
    return it == meshes.end() ? nullptr : it->second;
}

std::shared_ptr<Texture> AssetManager::getTexture(AssetID id) const
{
    auto it = textures.find(id);
    return it == textures.end() ? nullptr : it->second;
}

std::shared_ptr<Material> AssetManager::getMaterial(AssetID id) const
{
    auto it = materials.find(id);
    return it == materials.end() ? nullptr : it->second;
}

std::shared_ptr<Shader> AssetManager::getShader(AssetID id) const
{
    auto it = shaders.find(id);
    return it == shaders.end() ? nullptr : it->second;
}

const AssetMetadata* AssetManager::getMetadata(AssetID id) const
{
    auto it = metadata.find(id);
    return it == metadata.end() ? nullptr : &it->second;
}

void AssetManager::addMetadata(AssetID id, AssetType type, const std::string& name)
{
    metadata[id] = AssetMetadata{
        id,
        type,
        name,
        {},
        {},
        false
    };
}

std::string AssetManager::getLowerExtension(const std::string& path)
{
    std::string extension = path.substr(path.find_last_of('.') + 1);
    std::transform(extension.begin(), extension.end(), extension.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return extension;
}

void AssetManager::clearAssets()
{
    metadata.clear();
    models.clear();
    meshes.clear();
    textures.clear();
    materials.clear();
    shaders.clear();
}
