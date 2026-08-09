#include "gltf_importer.hpp"

#include <cstring>
#include <map>
#include <utility>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "core/console/console.hpp"
#include "core/project/asset_manager.hpp"
#include "core/project/asset_defaults.hpp"
#include "graphics/material/material.hpp"
#include "graphics/models/model.hpp"
#include "graphics/texture/texture.hpp"
#include "utils.hpp"

GLTFImporter::GLTFImporter(
    std::shared_ptr<ModelAssetManager> modelManager,
    std::shared_ptr<MeshAssetManager> meshManager,
    std::shared_ptr<MaterialAssetManager> materialManager,
    std::shared_ptr<TextureAssetManager> textureManager,
    std::shared_ptr<AssetDefaults> assetDefaults
) : modelManager(modelManager),
    meshManager(meshManager),
    materialManager(materialManager),
    textureManager(textureManager),
    assetDefaults(assetDefaults)
{
}

std::shared_ptr<Model> GLTFImporter::import(const std::string& name, const std::string& filePath)
{
    currentFilePath = filePath;

    if (!doesFileExist(filePath.c_str())) {
        Console::get().error("[GLTFImporter::import] Invalid file path: '" + currentFilePath + "'");
        resetImportState();
        return nullptr;
    }

    if (getFileExtension(filePath.c_str()) != ".gltf") {
        Console::get().error("[GLTFImporter::import] Invalid file extension (expected .gltf): '" + currentFilePath + "'");
        resetImportState();
        return nullptr;
    }

    std::string text = readFile(currentFilePath.c_str());
    jsonContents = json::parse(text);

    std::string uri = jsonContents["buffers"][0]["uri"];
    const size_t directoryEnd = currentFilePath.find_last_of("/\\");
    currentFileDirectory = directoryEnd == std::string::npos ? "" : currentFilePath.substr(0, directoryEnd + 1);
    std::string binaryContents = readFile((currentFileDirectory + uri).c_str());
    binaryData = std::vector<unsigned char>(binaryContents.begin(), binaryContents.end());

    Model model;
    model.setFilePath(currentFilePath);
    currentModel = &model;

    if (!jsonContents.contains("scenes") || jsonContents["scenes"].empty()) {
        Console::get().warn("[GLTFImporter::import] No scenes found in glTF file: '" + currentFilePath + "'");
        resetImportState();
        return nullptr;
    }

    unsigned int sceneIndex = jsonContents.value("scene", 0);
    if (sceneIndex >= jsonContents["scenes"].size()) {
        Console::get().warn("[GLTFImporter::import] Default scene index out of range, using scene 0: '" + currentFilePath + "'");
        sceneIndex = 0;
    }

    const json& scene = jsonContents["scenes"][sceneIndex];
    if (!scene.contains("nodes")) {
        Console::get().warn("[GLTFImporter::import] Scene contains no nodes: '" + currentFilePath + "'");
        resetImportState();
        return nullptr;
    }

    for (const auto& nodeIndex : scene["nodes"]) {
        traverseNode(nodeIndex);
    }

    std::shared_ptr<Model> modelAsset = modelManager->add(name, model);
    modelManager->setSourcePath(modelAsset, filePath);

    resetImportState();

    return modelAsset;
}

void GLTFImporter::resetImportState()
{
    currentModel = nullptr;
    currentFileDirectory.clear();
    currentFilePath.clear();
    binaryData.clear();
    jsonContents.clear();
}

void GLTFImporter::traverseNode(unsigned int nextNode, glm::mat4 parentTransMatrix)
{
    const json& node = jsonContents["nodes"][nextNode];

    glm::mat4 localTransform = glm::mat4(1.0f);

    if (node.contains("matrix")) {
        std::vector<float> matrixValues = node["matrix"].get<std::vector<float>>();
        if (matrixValues.size() == 16) {
            localTransform = glm::make_mat4(matrixValues.data());
        }
    } else {
        std::vector<float> translationValues = node.value("translation", std::vector<float>{0.0f, 0.0f, 0.0f});
        std::vector<float> rotationValues = node.value("rotation", std::vector<float>{0.0f, 0.0f, 0.0f, 1.0f});
        std::vector<float> scaleValues = node.value("scale", std::vector<float>{1.0f, 1.0f, 1.0f});

        glm::vec3 translation(translationValues[0], translationValues[1], translationValues[2]);
        glm::quat rotation(rotationValues[3], rotationValues[0], rotationValues[1], rotationValues[2]);
        glm::vec3 scale(scaleValues[0], scaleValues[1], scaleValues[2]);

        localTransform = glm::translate(glm::mat4(1.0f), translation) * glm::mat4_cast(rotation) * glm::scale(glm::mat4(1.0f), scale);
    }

    glm::mat4 globalTransform = parentTransMatrix * localTransform;

    if (node.contains("mesh")) {
        loadMesh(node["mesh"], globalTransform);
    }

    if (node.contains("children")) {
        for (const auto& childNodeIndex : node["children"]) {
            traverseNode(childNodeIndex, globalTransform);
        }
    }
}

std::vector<float> GLTFImporter::readAccessorFloats(json accessor)
{
    std::vector<float> floatVector;

    unsigned int bufferViewIndex = accessor.value("bufferView", 0);
    unsigned int nComponents = accessor["count"];
    unsigned int byteOffset = accessor.value("byteOffset", 0);
    std::string componentType = accessor["type"];

    std::map<std::string, unsigned int> typeToComponentCount = {
        {"SCALAR", 1},
        {"VEC2", 2},
        {"VEC3", 3},
        {"VEC4", 4}
    };
    auto it = typeToComponentCount.find(componentType);
    if (it == typeToComponentCount.end()) {
        Console::get().error("[GLTFImporter::readAccessorFloats] Invalid component type '" + componentType + "'");
        return floatVector;
    }

    unsigned int componentCount = it->second;
    unsigned int viewByteOffset = jsonContents["bufferViews"][bufferViewIndex].value("byteOffset", 0);

    unsigned int dataStart = byteOffset + viewByteOffset;
    unsigned int nFloats = componentCount*nComponents;
    unsigned int dataEnd = dataStart + sizeof(float)*nFloats;

    if (dataEnd > binaryData.size()) {
        Console::get().error("[GLTFImporter::readAccessorFloats] Attempting to read out-of-bounds data: '" + currentFilePath + "'");
        return floatVector;
    }

    const float* floats = reinterpret_cast<const float*>(binaryData.data() + dataStart);
    floatVector.assign(floats, floats + nFloats);

    return floatVector;
}

std::vector<unsigned int> GLTFImporter::readAccessorIndices(json accessor)
{
    std::vector<unsigned int> indices;

    unsigned int bufferViewIndex = accessor.value("bufferView", 0);
    unsigned int nComponents = accessor["count"];
    unsigned int byteOffset = accessor.value("byteOffset", 0);
    unsigned int componentType = accessor["componentType"];
    unsigned int viewByteOffset = jsonContents["bufferViews"][bufferViewIndex].value("byteOffset", 0);
    unsigned int beginningOfData = byteOffset + viewByteOffset;

    enum ComponentType : unsigned int {
        BYTE = 5120,
        UNSIGNED_BYTE = 5121,
        SHORT = 5122,
        UNSIGNED_SHORT = 5123,
        INT = 5124,
        UNSIGNED_INT = 5125,
        FLOAT = 5126
    };

    unsigned int nBytesPerComponent = 0;
    switch ((ComponentType)componentType) {
        case ComponentType::BYTE:
            Console::get().error("[GLTFImporter::readAccessorIndices] BYTE (5120) is not supported for index accessors: '" + currentFilePath + "'");
            return indices;
        case ComponentType::UNSIGNED_BYTE:
            Console::get().error("[GLTFImporter::readAccessorIndices] UNSIGNED_BYTE (5121) is not supported for index accessors: '" + currentFilePath + "'");
            return indices;
        case ComponentType::SHORT:
            nBytesPerComponent = sizeof(short);
            break;
        case ComponentType::UNSIGNED_SHORT:
            nBytesPerComponent = sizeof(unsigned short);
            break;
        case ComponentType::INT:
            nBytesPerComponent = sizeof(int);
            break;
        case ComponentType::UNSIGNED_INT:
            nBytesPerComponent = sizeof(unsigned int);
            break;
        case ComponentType::FLOAT:
            Console::get().error("[GLTFImporter::readAccessorIndices] FLOAT (5126) is not supported for index accessors: '" + currentFilePath + "'");
            return indices;
        default:
            Console::get().error("[GLTFImporter::readAccessorIndices] Invalid component type '" + std::to_string(componentType) + "' in accessor for indices: '" + currentFilePath + "'");
            return indices;
    }

    unsigned int dataEnd = beginningOfData + nComponents*nBytesPerComponent;
    if (dataEnd > binaryData.size()) {
        Console::get().error("[GLTFImporter::readAccessorIndices] Attempting to read out-of-bounds data: '" + currentFilePath + "'");
        return indices;
    }

    indices.reserve(nComponents);
    for (unsigned int ic = 0; ic < nComponents; ic++) {
        unsigned int index = 0;
        std::memcpy(&index, binaryData.data() + beginningOfData + ic*nBytesPerComponent, nBytesPerComponent);
        indices.push_back(index);
    }

    return indices;
}

std::string GLTFImporter::getTexturePathFromUri(unsigned int textureIndex) const
{
    if (!jsonContents.contains("textures") || textureIndex >= jsonContents["textures"].size()) {
        Console::get().error("[GLTFImporter::getTexturePathFromUri] Invalid texture index: '" + std::to_string(textureIndex) + "'");
        return "";
    }

    const json& texture = jsonContents["textures"][textureIndex];
    if (!texture.contains("source")) {
        Console::get().error("[GLTFImporter::getTexturePathFromUri] Texture does not contain a source: '" + std::to_string(textureIndex) + "'");
        return "";
    }

    unsigned int imageIndex = texture["source"];
    if (!jsonContents.contains("images") || imageIndex >= jsonContents["images"].size()) {
        Console::get().error("[GLTFImporter::getTexturePathFromUri] Invalid image index: '" + std::to_string(imageIndex) + "'");
        return "";
    }

    const json& image = jsonContents["images"][imageIndex];
    if (!image.contains("uri")) {
        Console::get().error("[GLTFImporter::getTexturePathFromUri] Image does not contain a uri: '" + std::to_string(imageIndex) + "'");
        return "";
    }

    std::string uri = image["uri"];
    return currentFileDirectory + uri;
}

std::shared_ptr<Mesh> GLTFImporter::loadMesh(unsigned int meshIndex, const glm::mat4& initialTransform)
{
    const json& primitives = jsonContents["meshes"][meshIndex]["primitives"];
    if (primitives.empty()) {
        Console::get().error("[GLTFImporter::loadMesh] Mesh " + std::to_string(meshIndex) + " contains no primitives.");
        return nullptr;
    }

    const json& attributes = primitives[0]["attributes"];
    unsigned int positionAccessorIndex = attributes["POSITION"];
    unsigned int normalAccessorIndex = attributes["NORMAL"];
    unsigned int texCoordAccessorIndex = attributes["TEXCOORD_0"];
    unsigned int indexAccessorIndex = primitives[0]["indices"];

    std::vector<glm::vec3> positions = toVec3(readAccessorFloats(jsonContents["accessors"][positionAccessorIndex]));
    std::vector<glm::vec3> normals = toVec3(readAccessorFloats(jsonContents["accessors"][normalAccessorIndex]));
    std::vector<glm::vec2> textureUVs = toVec2(readAccessorFloats(jsonContents["accessors"][texCoordAccessorIndex]));
    std::vector<glm::vec4> tangents;
    std::vector<unsigned int> indices = readAccessorIndices(jsonContents["accessors"][indexAccessorIndex]);

    const int materialIndex = primitives[0].value("material", -1);
    std::shared_ptr<Material> material = loadMaterial(materialIndex);
    if (!material) {
        material = assetDefaults->getDefaultMaterial();
    }

    Mesh mesh(positions, normals, textureUVs, tangents, indices);

    std::shared_ptr<Mesh> meshAsset = addMesh("Mesh_" + std::to_string(meshIndex), mesh);
    if (currentModel) {
        currentModel->addPrimitive(meshAsset, std::move(material), initialTransform);
    }

    return meshAsset;
}

std::shared_ptr<Mesh> GLTFImporter::addMesh(const std::string& name, const Mesh& mesh)
{
    return meshManager->add(name, mesh);
}

std::shared_ptr<Material> GLTFImporter::loadMaterial(int materialIndex)
{
    Material material;

    if (materialIndex < 0 || !jsonContents.contains("materials")) {
        return nullptr;
    }
    if (materialIndex >= jsonContents["materials"].size()) {
        Console::get().warn("[GLTFImporter::loadMaterial] Invalid material index: '" + std::to_string(materialIndex) + "'");
        return nullptr;
    }

    const json& materialJson = jsonContents["materials"][materialIndex];

    auto addTextureIfPresent = [&](const json& parent, const char* key, TextureType type) {
        if (!parent.contains(key)) {
            return;
        }
        const json& textureInfo = parent[key];
        if (!textureInfo.contains("index")) {
            return;
        }

        std::string uri = getTexturePathFromUri(textureInfo["index"]);
        if (uri.empty()) {
            return;
        }

        std::shared_ptr<Texture> texture = textureManager->add(uri, Texture(uri, type));
        textureManager->setSourcePath(texture, uri);
        switch (type) {
            case TextureType::DIFFUSE:
                material.setAlbedoTexture(texture);
                break;
            case TextureType::SPECULAR:
                material.setSpecularTexture(texture);
                break;
            case TextureType::NORMAL:
                material.setNormalTexture(texture);
                break;
        }
    };

    if (materialJson.contains("pbrMetallicRoughness")) {
        const json& pbr = materialJson["pbrMetallicRoughness"];
        addTextureIfPresent(pbr, "baseColorTexture", TextureType::DIFFUSE);
        addTextureIfPresent(pbr, "metallicRoughnessTexture", TextureType::SPECULAR);
    }
    addTextureIfPresent(materialJson, "normalTexture", TextureType::NORMAL);

    return addMaterial("Material_" + std::to_string(materialIndex), material);
}

std::shared_ptr<Material> GLTFImporter::addMaterial(const std::string& name, const Material& material)
{
    return materialManager->add(name, assetDefaults->applyToMaterial(material));
}

std::vector<glm::vec2> GLTFImporter::toVec2(const std::vector<float> floatVec)
{
    if (floatVec.size() % 2 != 0) {
        Console::get().error("[GLTFImporter::toVec2] Float vector size is not a multiple of 2: '" + currentFilePath + "'");
        return {};
    }

    std::vector<glm::vec2> vectors;
    vectors.reserve(floatVec.size() / 2);
    for (unsigned int iFloat = 0; iFloat < floatVec.size(); iFloat += 2) {
        vectors.emplace_back(floatVec[iFloat], floatVec[iFloat + 1]);
    }

    return vectors;
}

std::vector<glm::vec3> GLTFImporter::toVec3(const std::vector<float> floatVec)
{
    if (floatVec.size() % 3 != 0) {
        Console::get().error("[GLTFImporter::toVec3] Float vector size is not a multiple of 3: '" + currentFilePath + "'");
        return {};
    }

    std::vector<glm::vec3> vectors;
    vectors.reserve(floatVec.size() / 3);
    for (unsigned int iFloat = 0; iFloat < floatVec.size(); iFloat += 3) {
        vectors.emplace_back(floatVec[iFloat], floatVec[iFloat + 1], floatVec[iFloat + 2]);
    }

    return vectors;
}

std::vector<glm::vec4> GLTFImporter::toVec4(const std::vector<float> floatVec)
{
    if (floatVec.size() % 4 != 0) {
        Console::get().error("[GLTFImporter::toVec4] Float vector size is not a multiple of 4: '" + currentFilePath + "'");
        return {};
    }

    std::vector<glm::vec4> vectors;
    vectors.reserve(floatVec.size() / 4);
    for (unsigned int iFloat = 0; iFloat < floatVec.size(); iFloat += 4) {
        vectors.emplace_back(floatVec[iFloat], floatVec[iFloat + 1], floatVec[iFloat + 2], floatVec[iFloat + 3]);
    }

    return vectors;
}
