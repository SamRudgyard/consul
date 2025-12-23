#include "model.hpp"
#include "graphics/shaders/shader.hpp"
#include "graphics/camera/camera.hpp"
#include "graphics/mesh/mesh.hpp"
#include "graphics/mesh/renderable_mesh.hpp"
#include "graphics/textures/renderable_texture.hpp"
#include "core/console/console.hpp"
#include "utils.hpp"

#include <algorithm>

Model::Model(const char* modelPath)
    : file(std::string(modelPath))
{
	// Make a JSON object
	std::string text = readFile(modelPath);
	jsonContents = json::parse(text);

    if (getFileExtension(modelPath) != ".gltf") {
        Console::get().error("[Model::Model] Unsupported model format in file: '" + file + "'. Only .gltf files are supported.");
    }

    // The uri (unique resource identifier) of the binary data
	std::string uri = jsonContents["buffers"][0]["uri"];

	fileDirectory = file.substr(0, file.find_last_of('/') + 1);
	std::string binaryContents = readFile((fileDirectory + uri).c_str());
    binaryData = std::vector<unsigned char>(binaryContents.begin(), binaryContents.end());

    unsigned int sceneIndex = 0;

    if (!jsonContents.contains("scenes") || jsonContents["scenes"].empty()) {
        Console::get().warn("[Model::Model] No scenes found in glTF file: '" + file + "'");
        traverseNode(sceneIndex);
        return;
    }

    sceneIndex = jsonContents.value("scene", 0);
    if (sceneIndex >= jsonContents["scenes"].size()) {
        Console::get().warn("[Model::Model] Default scene index out of range, using scene 0: '" + file + "'");
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

std::vector<glm::mat4> Model::getTransformationMatrices() const
{
    std::vector<glm::mat4> combinedTransforms;
    combinedTransforms.reserve(transformationMatrices.size());
    for (const glm::mat4& baseTransform : transformationMatrices) {
        combinedTransforms.push_back(modelTransform * baseTransform);
    }
    return combinedTransforms;
}

void Model::translate(const glm::vec3& delta)
{
    modelTransform = glm::translate(modelTransform, delta);
}

void Model::rotate(float angleRadians, const glm::vec3& axis)
{
    modelTransform = glm::rotate(modelTransform, angleRadians, axis);
}

void Model::scale(const glm::vec3& factor)
{
    modelTransform = glm::scale(modelTransform, factor);
}

void Model::setTransform(const glm::mat4& transform)
{
    modelTransform = transform;
}

void Model::resetTransform()
{
    modelTransform = glm::mat4(1.0f);
}

void Model::loadMesh(unsigned int iMesh)
{
	// Get all accessor indices
    json primitives = jsonContents["meshes"][iMesh]["primitives"];
    if (primitives.size() < 1) {
        Console::get().error("[Model::loadMesh] Mesh " + std::to_string(iMesh) + " contains no primitives.");
    }
    json attributes = primitives[0]["attributes"];
	unsigned int positionAccessorIndex = attributes["POSITION"];
	unsigned int normalAccessorIndex = attributes["NORMAL"];
	unsigned int texCoordAccessorIndex = attributes["TEXCOORD_0"];
	unsigned int indexAccessorIndex = primitives[0]["indices"];

	std::vector<float> positionFloats = readAccessorFloats(jsonContents["accessors"][positionAccessorIndex]);
	std::vector<glm::vec3> positions = toVec3(positionFloats);
	std::vector<float> normalFloats = readAccessorFloats(jsonContents["accessors"][normalAccessorIndex]);
	std::vector<glm::vec3> normals = toVec3(normalFloats);
    std::vector<glm::vec4> colours; // Empty colours vector, TODO: implement reading colors if needed
    std::vector<float> textureFloats = readAccessorFloats(jsonContents["accessors"][texCoordAccessorIndex]);
    std::vector<glm::vec2> textureUVs = toVec2(textureFloats);
    std::vector<glm::vec4> tangents; // Empty tangents vector, TODO: implement reading tangents if needed
	std::vector<unsigned int> indices = readAccessorIndices(jsonContents["accessors"][indexAccessorIndex]);

    int materialIndex = primitives[0].value("material", -1);
    std::vector<Texture> textures = getTexturesForMaterial(materialIndex);

    Mesh mesh(positions, normals, colours, textureUVs, tangents, indices, textures);
    meshes.push_back(mesh); // TODO: Remove when we get rid of mesh owning its textures
}

void Model::traverseNode(unsigned int nextNode, glm::mat4 parentTransMatrix)
{
    glm::mat4 localTransMatrix = glm::mat4(1.0f);

    json node = jsonContents["nodes"][nextNode];

    if (node.contains("matrix"))
    {
        localTransMatrix = glm::make_mat4(node["matrix"].get<std::vector<float>>().data());
    } else {
        // If there is no local transformation matrix, check for translation, rotation, and scale

        std::vector<float> tVec = node.value("translation", std::vector<float>{0.0f, 0.0f, 0.0f});
        std::vector<float> rVec = node.value("rotation", std::vector<float>{0.0f, 0.0f, 0.0f, 1.0f});
        std::vector<float> sVec = node.value("scale", std::vector<float>{1.0f, 1.0f, 1.0f});

        glm::vec3 t = glm::vec3(tVec[0], tVec[1], tVec[2]);
        glm::quat r = glm::quat(rVec[3], rVec[0], rVec[1], rVec[2]); // NOTE: glm::quat(w, x, y, z) NOT glm::quat(x, y, z, w)
        glm::vec3 s = glm::vec3(sVec[0], sVec[1], sVec[2]);

        localTransMatrix = glm::translate(glm::mat4(1.0f), t) * glm::mat4_cast(r) * glm::scale(glm::mat4(1.0f), s);
    }

    glm::mat4 transformationMatrix = parentTransMatrix * localTransMatrix;

	if (node.contains("mesh"))
	{
        // Only store the transformation matrix if there is a mesh to go with it
        transformationMatrices.push_back(transformationMatrix);
		loadMesh(node["mesh"]);
	}

	if (node.contains("children"))
	{
		for (unsigned int ic = 0; ic < node["children"].size(); ic++)
			traverseNode(node["children"][ic], transformationMatrix);
	}
}

std::vector<float> Model::readAccessorFloats(json accessor)
{
	std::vector<float> floatVector;

	// Get properties from the accessor
	unsigned int bufferViewIndex = accessor.value("bufferView", 0);
	unsigned int nComponents = accessor["count"];
	unsigned int byteOffset = accessor.value("byteOffset", 0);
	std::string componentType = accessor["type"];

    std::map<std::string, unsigned int> typeToBytesPerComponent = {
        {"SCALAR", 1},
        {"VEC2", 2},
        {"VEC3", 3},
        {"VEC4", 4}
    };
    auto it = typeToBytesPerComponent.find(componentType);
    if (it == typeToBytesPerComponent.end()) {
        Console::get().error("[Model::readAccessorFloats] Invalid component type '" + componentType + "' in accessor (not SCALAR, VEC2, VEC3, or VEC4): '" + file + "'");
    }
    unsigned int bytesPerComponent = it->second;

	unsigned int viewByteOffset = jsonContents["bufferViews"][bufferViewIndex].value("byteOffset", 0);

	unsigned int dataStart = byteOffset + viewByteOffset;
    unsigned int nFloats = bytesPerComponent*nComponents;
	unsigned int dataEnd = dataStart + sizeof(float)*nFloats;

    if (dataEnd > binaryData.size()) {
        Console::get().error("[Model::readAccessorFloats] Attempting to read out-of-bounds data: '" + file + "'");
    }

    const float* floats = reinterpret_cast<const float*>(binaryData.data() + dataStart);
    floatVector.assign(floats, floats + nFloats);

	return floatVector;
}

std::vector<unsigned int> Model::readAccessorIndices(json accessor)
{
	std::vector<unsigned int> indices;

	// Get properties from the accessor
	unsigned int bufferViewIndex = accessor.value("bufferView", 0);
	unsigned int nComponents = accessor["count"];
	unsigned int byteOffset = accessor.value("byteOffset", 0);
	unsigned int componentType = accessor["componentType"];

    unsigned int viewByteOffset = jsonContents["bufferViews"][bufferViewIndex].value("byteOffset", 0);

	unsigned int beginningOfData = byteOffset + viewByteOffset;

    unsigned int nBytesPerComponent = 0;
    switch (componentType) {
        case GL_BYTE: // 5120
            Console::get().error("[Model::readAccessorIndices] GL_BYTE (5120) is not supported for index accessors: '" + file + "'");
            break;
        case GL_UNSIGNED_BYTE: // 5121
            Console::get().error("[Model::readAccessorIndices] GL_UNSIGNED_BYTE (5121) is not supported for index accessors: '" + file + "'");
            break;
        case GL_SHORT: // 5122
            nBytesPerComponent = sizeof(short);
            break;
        case GL_UNSIGNED_SHORT: // 5123
            nBytesPerComponent = sizeof(unsigned short);
            break;
        case GL_INT: // 5124
            nBytesPerComponent = sizeof(int);
            break;
        case GL_UNSIGNED_INT: // 5125
            nBytesPerComponent = sizeof(unsigned int);
            break;
        case GL_FLOAT: // 5126
            Console::get().error("[Model::readAccessorIndices] GL_FLOAT (5126) is not supported for index accessors: '" + file + "'");
            break;
        default:
            Console::get().error("[Model::readAccessorIndices] Invalid component type '" + std::to_string(componentType) + "' in accessor for indices: '" + file + "'");
            break;
    }

    for (unsigned int ic = 0; ic < nComponents; ic++) {
        unsigned int index = 0;
        std::memcpy(&index, binaryData.data() + beginningOfData + ic*nBytesPerComponent, nBytesPerComponent);
        indices.push_back(index);
    }

	return indices;
}

std::string Model::getTexturePathFromUri(unsigned int textureIndex) const
{
    if (!jsonContents.contains("textures") || textureIndex >= jsonContents["textures"].size()) {
        Console::get().error("[Model::getTexturePathFromUri] Invalid texture index: '" + std::to_string(textureIndex) + "'");
    }

    const json& texture = jsonContents["textures"][textureIndex];
    if (!texture.contains("source")) {
        Console::get().error("[Model::getTexturePathFromUri] Texture has no source: '" + std::to_string(textureIndex) + "'");
    }

    unsigned int imageIndex = texture["source"];
    if (!jsonContents.contains("images") || imageIndex >= jsonContents["images"].size()) {
        Console::get().error("[Model::getTexturePathFromUri] Invalid image index: '" + std::to_string(imageIndex) + "'");
    }

    const json& image = jsonContents["images"][imageIndex];
    if (!image.contains("uri")) {
        Console::get().error("[Model::getTexturePathFromUri] Image has no uri: '" + std::to_string(imageIndex) + "'");
    }

    std::string uri = image["uri"];
    return fileDirectory + uri;
}

std::vector<Texture> Model::getTexturesForMaterial(int materialIndex) const
{
    std::vector<Texture> textures;

    if (materialIndex < 0 || !jsonContents.contains("materials")) {
        return textures;
    }
    if (materialIndex >= jsonContents["materials"].size()) {
        Console::get().warn("[Model::getTexturesForMaterial] Invalid material index: '" + std::to_string(materialIndex) + "'");
        return textures;
    }

    const json& material = jsonContents["materials"][materialIndex];

    auto addTextureIfPresent = [&](const json& parent, const char* key, TextureType type) {
        if (!parent.contains(key)) {
            return;
        }
        const json& textureInfo = parent[key];
        if (!textureInfo.contains("index")) {
            return;
        }
        unsigned int textureIndex = textureInfo["index"];
        std::string uri = getTexturePathFromUri(textureIndex);
        if (!uri.empty()) {
            textures.emplace_back(Texture(uri, type));
        }
    };

    if (material.contains("pbrMetallicRoughness")) {
        const json& pbr = material["pbrMetallicRoughness"];
        addTextureIfPresent(pbr, "baseColorTexture", TextureType::DIFFUSE);
        addTextureIfPresent(pbr, "metallicRoughnessTexture", TextureType::SPECULAR);
    }

    addTextureIfPresent(material, "normalTexture", TextureType::NORMAL);

    return textures;
}

std::vector<glm::vec2> Model::toVec2(std::vector<float> floatVec)
{
	if (floatVec.size() % 2 != 0) {
        Console::get().error("[Model::toVec2] Float vector size is not a multiple of 2: '" + file + "'");
    }

	const unsigned int floatsPerVector = 2;
    const unsigned int nVectors = floatVec.size() / floatsPerVector;

	std::vector<glm::vec2> vectors;
    vectors.reserve(nVectors);

	for (unsigned int iFloat = 0; iFloat < floatVec.size(); iFloat += floatsPerVector)
	{
		vectors.emplace_back(floatVec[iFloat], floatVec[iFloat + 1]);
	}

	return vectors;
}

std::vector<glm::vec3> Model::toVec3(std::vector<float> floatVec)
{
    if (floatVec.size() % 3 != 0) {
        Console::get().error("[Model::toVec3] Float vector size is not a multiple of 3: '" + file + "'");
    }

	const unsigned int floatsPerVector = 3;
    const unsigned int nVectors = floatVec.size() / floatsPerVector;

	std::vector<glm::vec3> vectors;
    vectors.reserve(nVectors);

	for (unsigned int iFloat = 0; iFloat < floatVec.size(); iFloat += floatsPerVector)
	{
		vectors.emplace_back(floatVec[iFloat], floatVec[iFloat + 1], floatVec[iFloat + 2]);
	}

	return vectors;
}

std::vector<glm::vec4> Model::toVec4(std::vector<float> floatVec)
{
	if (floatVec.size() % 4 != 0) {
        Console::get().error("[Model::toVec4] Float vector size is not a multiple of 4: '" + file + "'");
    }

	const unsigned int floatsPerVector = 4;
    const unsigned int nVectors = floatVec.size() / floatsPerVector;

	std::vector<glm::vec4> vectors;
    vectors.reserve(nVectors);

	for (unsigned int iFloat = 0; iFloat < floatVec.size(); iFloat += floatsPerVector)
	{
		vectors.emplace_back(floatVec[iFloat], floatVec[iFloat + 1], floatVec[iFloat + 2], floatVec[iFloat + 3]);
	}

	return vectors;
}
