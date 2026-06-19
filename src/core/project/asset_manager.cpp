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

using json = nlohmann::json;

AssetID AssetManager::loadAsset(const std::string& name, const std::string& path)
{
    if (!doesFileExist(path.c_str())) {
        Console::get().warn("[AssetManager::loadAsset] Invalid asset path: '" + path + "'");
        return INVALID_ASSET_ID;
    }

    const std::string extension = getFileExtension(path.c_str());
    if (extension.empty()) {
        Console::get().warn("[AssetManager::loadAsset] Asset file has no extension: '" + path + "'");
        return INVALID_ASSET_ID;
    }

    for (const auto& loader : loaders) {
        if (loader->isSupportedExtension(extension)) {
            loader->load(path);
            return INVALID_ASSET_ID; // TODO: Return the actual AssetID after loading
        }
    }
}

AssetID AssetManager::loadFromGLTF(const std::string& name, const std::string& path)
{
    // Make a JSON object
	std::string text = readFile(path.c_str());
	json jsonContents = json::parse(text);

    // The uri (unique resource identifier) of the binary data
	std::string uri = jsonContents["buffers"][0]["uri"];

    std::string fileDirectory = path.substr(0, path.find_last_of('/') + 1);
	std::string binaryContents = readFile((fileDirectory + uri).c_str());
    std::vector<unsigned char> binaryData = std::vector<unsigned char>(binaryContents.begin(), binaryContents.end());

    if (!jsonContents.contains("scenes") || jsonContents["scenes"].empty()) {
        Console::get().warn("[AssetManager::loadFromGLTF] No scenes found in glTF file: '" + path + "'");
        return INVALID_ASSET_ID;
    }

    unsigned int sceneIndex = jsonContents.value("scene", 0);

    if (sceneIndex >= jsonContents["scenes"].size()) {
        Console::get().warn("[AssetManager::loadFromGLTF] Scene index (" + std::to_string(sceneIndex) + ") > number of scenes (" + std::to_string(jsonContents["scenes"].size() - 1) + ") in glTF file: '" + path + "'");
        return INVALID_ASSET_ID;
    }

    const auto& traverseNode = [&](unsigned int nodeIndex, glm::mat4 parentTransform = glm::mat4(1.0f)) {
        glm::mat4 localTransform = glm::mat4(1.0f);
        json node = jsonContents["nodes"][nodeIndex];

        if (node.contains("matrix")) {
            localTransform = glm::make_mat4(node["matrix"].get<std::vector<float>>().data());
        } else {
            // If there is no local transformation matrix, check for translation, rotation, and scale

            std::vector<float> tVec = node.value("translation", std::vector<float>{0.0f, 0.0f, 0.0f});
            std::vector<float> rVec = node.value("rotation", std::vector<float>{0.0f, 0.0f, 0.0f, 1.0f});
            std::vector<float> sVec = node.value("scale", std::vector<float>{1.0f, 1.0f, 1.0f});

            glm::vec3 t = glm::vec3(tVec[0], tVec[1], tVec[2]);
            glm::quat r = glm::quat(rVec[3], rVec[0], rVec[1], rVec[2]); // NOTE: glm::quat(w, x, y, z) NOT glm::quat(x, y, z, w)
            glm::vec3 s = glm::vec3(sVec[0], sVec[1], sVec[2]);

            localTransform = glm::translate(glm::mat4(1.0f), t) * glm::mat4_cast(r) * glm::scale(glm::mat4(1.0f), s);
        }

        glm::mat4 transformation = parentTransform * localTransform;

        if (node.contains("mesh")) {
            loadMesh(node["mesh"], transformation);
        }

        if (node.contains("children")) {
            for (unsigned int ic = 0; ic < node["children"].size(); ic++)
                traverseNode(node["children"][ic], transformation);
        }
    };

    for (const auto& nodeIndex : jsonContents["scenes"][sceneIndex]["nodes"]) {
        traverseNode(nodeIndex);
    }
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
    meshes[id] = std::make_shared<Mesh>(mesh);
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
    materials[id] = std::make_shared<Material>(material);
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

AssetID AssetManager::loadModelFromGLTF(const std::string& name, const std::string& path)
{
    
}