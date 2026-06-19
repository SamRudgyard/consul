#include "gltf_importer.hpp"

#include <glm/gtc/type_ptr.hpp>

#include "core/console/console.hpp"

AssetID GLTFImporter::import(const std::string& filePath)
{
    if (!doesFileExist(filePath.c_str())) {
        Console::get().error("[GLTFImporter::import] Invalid file path: '" + filePath + "'");
        return INVALID_ASSET_ID;
    }

    if (getFileExtension(filePath.c_str()) != ".gltf") {
        Console::get().error("[GLTFImporter::import] Invalid file extension (expected .gltf): '" + filePath + "'");
        return INVALID_ASSET_ID;
    }

    if (assetManager.expired()) {
        Console::get().error("[GLTFImporter::import] Asset manager is not assigned!");
        return INVALID_ASSET_ID;
    }

    assetManager.lock();

    // Make a JSON object
	std::string text = readFile(filePath.c_str());
	jsonContents = json::parse(text);

    // The uri (unique resource identifier) of the binary data
    std::string uri = jsonContents["buffers"][0]["uri"];
    
	fileDirectory = filePath.substr(0, filePath.find_last_of('/') + 1);
	std::string binaryContents = readFile((fileDirectory + uri).c_str());
    binaryData = std::vector<unsigned char>(binaryContents.begin(), binaryContents.end());

    unsigned int sceneIndex = 0;

    if (!jsonContents.contains("scenes") || jsonContents["scenes"].empty()) {
        Console::get().warn("[Model::Model] No scenes found in glTF file: '" + filePath + "'");
        traverseNode(sceneIndex);
        return;
    }

    sceneIndex = jsonContents.value("scene", 0);
    if (sceneIndex >= jsonContents["scenes"].size()) {
        Console::get().warn("[Model::Model] Default scene index out of range, using scene 0: '" + filePath + "'");
        sceneIndex = 0;
    }

    const json& scene = jsonContents["scenes"][sceneIndex];
    if (!scene.contains("nodes")) {
        traverseNode(sceneIndex);
        return;
    }

    for (const auto& nodeIndex : scene["nodes"]) {
        traverseNode(nodeIndex);
    }
}

void GLTFImporter::traverseNode(unsigned int nextNode, glm::mat4 parentTransMatrix)
{
    json node = jsonContents["nodes"][nextNode];

    glm::mat4 localTransform = glm::mat4(1.0f);

    if (node.contains("matrix")) {
        std::vector<float> matrixValues = readAccessorFloats(node["matrix"]);
        localTransform = glm::make_mat4(matrixValues.data());
    } else {
        if (node.contains("translation")) {
            std::vector<float> translationValues = readAccessorFloats(node["translation"]);
            localTransform = glm::translate(localTransform, glm::vec3(translationValues[0], translationValues[1], translationValues[2]));
        }
        if (node.contains("rotation")) {
            std::vector<float> rotationValues = readAccessorFloats(node["rotation"]);
            glm::quat rotationQuat(rotationValues[3], rotationValues[0], rotationValues[1], rotationValues[2]);
            localTransform *= glm::mat4_cast(rotationQuat);
        }
        if (node.contains("scale")) {
            std::vector<float> scaleValues = readAccessorFloats(node["scale"]);
            localTransform = glm::scale(localTransform, glm::vec3(scaleValues[0], scaleValues[1], scaleValues[2]));
        }
    }

    glm::mat4 globalTransMatrix = parentTransMatrix * localTransform;

    if (node.contains("mesh")) {
        unsigned int meshIndex = node["mesh"];
        // Load the mesh and create an AssetID for it
        // You can implement the logic to load the mesh and add it to the AssetManager here
    }

    if (node.contains("children")) {
        for (const auto& childNodeIndex : node["children"]) {
            traverseNode(childNodeIndex, globalTransMatrix);
        }
    }
}