#include "core/project/asset_manager.hpp"

#include <algorithm>
#include <cctype>
#include <utility>

#include "core/console/console.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "graphics/material/material.hpp"
#include "graphics/mesh/mesh.hpp"
#include "graphics/models/model.hpp"
#include "graphics/shader/shader.hpp"
#include "graphics/texture/texture.hpp"
#include "utils.hpp"

AssetID AssetManager::getDefaultMaterial()
{
    static AssetID defaultMaterialID = INVALID_ASSET_ID;
    if (defaultMaterialID == INVALID_ASSET_ID || !getMaterial(defaultMaterialID)) {
        defaultMaterialID = addMaterial("Default Material", Material());
    }
    return defaultMaterialID;
}

AssetID AssetManager::getDefaultTexture()
{
    static AssetID defaultTextureID = INVALID_ASSET_ID;
    if (defaultTextureID == INVALID_ASSET_ID || !getTexture(defaultTextureID)) {
        defaultTextureID = addTexture("Default Texture", Texture());
    }
    return defaultTextureID;
}

AssetID AssetManager::addModel(const std::string& name, const Model& model)
{
    AssetID id;
    models[id] = std::make_shared<Model>(model);
    addMetadata(id, AssetType::MODEL, name);
    return id;
}

AssetID AssetManager::addMesh(const std::string& name, const Mesh& mesh)
{
    AssetID id;
    Mesh meshWithDefaults = mesh;
    if (meshWithDefaults.getMaterial() == INVALID_ASSET_ID) {
        meshWithDefaults.setMaterial(getDefaultMaterial());
    }
    meshes[id] = std::make_shared<Mesh>(meshWithDefaults);
    addMetadata(id, AssetType::MESH, name);
    return id;
}

AssetID AssetManager::addTexture(const std::string& name, const Texture& texture)
{
    AssetID id;
    textures[id] = std::make_shared<Texture>(texture);
    addMetadata(id, AssetType::TEXTURE, name);
    return id;
}

AssetID AssetManager::addMaterial(const std::string& name, const Material& material)
{
    AssetID id;
    Material materialWithDefaults = material;
    if (materialWithDefaults.getAlbedoTextureID() == INVALID_ASSET_ID) {
        materialWithDefaults.setAlbedoTextureID(getDefaultTexture());
    }
    if (materialWithDefaults.getSpecularTextureID() == INVALID_ASSET_ID) {
        materialWithDefaults.setSpecularTextureID(getDefaultTexture());
    }
    materials[id] = std::make_shared<Material>(materialWithDefaults);
    addMetadata(id, AssetType::MATERIAL, name);
    return id;
}

AssetID AssetManager::addShader(const std::string& name, const Shader& shader)
{
    AssetID id;
    shaders[id] = std::make_shared<Shader>(shader);
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
    if (extension == ".gltf") {
        id = loadModelFromGLTF(name, path);
    } else {
        Console::get().warn("[AssetManager::importAsset] Unsupported asset extension: '" + extension + "'");
        return INVALID_ASSET_ID;
    }

    if (id == INVALID_ASSET_ID) {
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

    id = addShader(name, Shader(vertexPath.c_str(), fragmentPath.c_str()));
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

AssetID AssetManager::loadModelFromGLTF(const std::string& name, const std::string& path)
{
    return gltfImporter.import(*this, name, path);
}
