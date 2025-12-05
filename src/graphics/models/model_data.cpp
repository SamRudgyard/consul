#include "model_data.hpp"
#include "graphics/shaders/shader.hpp"
#include "graphics/camera/camera.hpp"
#include "graphics/models/mesh/mesh_data.hpp"
#include "graphics/textures/texture.hpp"
#include "core/console/console.hpp"
#include "utils.hpp"

#include <algorithm>

ModelData::ModelData(const char* modelPath)
    : file(std::string(modelPath))
{
	// Make a JSON object
	std::string text = readFile(modelPath);
	jsonContents = json::parse(text);

    if (getFileExtension(modelPath) != ".gltf") {
        Console::get().error("[ModelData::ModelData] Unsupported model format in file: '" + file + "'. Only .gltf files are supported.");
    }

    // The uri (unique resource identifier) of the binary data
	std::string uri = jsonContents["buffers"][0]["uri"];

	std::string fileDirectory = file.substr(0, file.find_last_of('/') + 1);
	std::string binaryContents = readFile((fileDirectory + uri).c_str());
    binaryData = std::vector<unsigned char>(binaryContents.begin(), binaryContents.end());

    loadTextures();

	// Traverse all nodes
	traverseNode(0);
}

void ModelData::draw(const IShader* shader, const Camera& camera, const glm::vec3 translation, const glm::quat rotation, const glm::vec3 scale)
{
    glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), scale);
    glm::mat4 rotationMatrix = glm::mat4_cast(rotation);
    glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), translation);

    glm::mat4 srtMatrix = scaleMatrix*rotationMatrix*translationMatrix;

	for (unsigned int im = 0; im < meshes.size(); im++)
	{
        meshes[im].setModelMatrix(srtMatrix * transformationMatrices[im]);
		// meshes[im].draw(shader, camera);
	}
}

MeshData& ModelData::loadMesh(unsigned int iMesh)
{
	// Get all accessor indices
    json primitives = jsonContents["meshes"][iMesh]["primitives"];
    if (primitives.size() < 1) {
        Console::get().error("[ModelData::loadMesh] Mesh " + std::to_string(iMesh) + " contains no primitives.");
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

    MeshData& mesh = meshes.emplace_back(MeshData(positions, normals, colours, textureUVs, tangents, indices, loadedTextures)); // TODO: Remove when we get rid of mesh owning its textures
    return mesh;
}

void ModelData::traverseNode(unsigned int nextNode, glm::mat4 parentTransMatrix)
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
		MeshData& mesh = loadMesh(node["mesh"]);
	}

	if (node.contains("children"))
	{
		for (unsigned int ic = 0; ic < node["children"].size(); ic++)
			traverseNode(node["children"][ic], transformationMatrix);
	}
}

std::vector<float> ModelData::readAccessorFloats(json accessor)
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
        Console::get().error("[ModelData::readAccessorFloats] Invalid component type '" + componentType + "' in accessor (not SCALAR, VEC2, VEC3, or VEC4): '" + file + "'");
    }
    unsigned int bytesPerComponent = it->second;

	unsigned int viewByteOffset = jsonContents["bufferViews"][bufferViewIndex].value("byteOffset", 0);

	unsigned int dataStart = byteOffset + viewByteOffset;
    unsigned int nFloats = bytesPerComponent*nComponents;
	unsigned int dataEnd = dataStart + sizeof(float)*nFloats;

    if (dataEnd > binaryData.size()) {
        Console::get().error("[ModelData::readAccessorFloats] Attempting to read out-of-bounds data: '" + file + "'");
    }

    const float* floats = reinterpret_cast<const float*>(binaryData.data() + dataStart);
    floatVector.assign(floats, floats + nFloats);

	return floatVector;
}

std::vector<unsigned int> ModelData::readAccessorIndices(json accessor)
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
            Console::get().error("[ModelData::readAccessorIndices] GL_BYTE (5120) is not supported for index accessors: '" + file + "'");
            break;
        case GL_UNSIGNED_BYTE: // 5121
            Console::get().error("[ModelData::readAccessorIndices] GL_UNSIGNED_BYTE (5121) is not supported for index accessors: '" + file + "'");
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
            Console::get().error("[ModelData::readAccessorIndices] GL_FLOAT (5126) is not supported for index accessors: '" + file + "'");
            break;
        default:
            Console::get().error("[ModelData::readAccessorIndices] Invalid component type '" + std::to_string(componentType) + "' in accessor for indices: '" + file + "'");
            break;
    }

    for (unsigned int ic = 0; ic < nComponents; ic++) {
        unsigned int index = 0;
        std::memcpy(&index, binaryData.data() + beginningOfData + ic*nBytesPerComponent, nBytesPerComponent);
        indices.push_back(index);
    }

	return indices;
}

void ModelData::loadTextures()
{
    const std::string fileDirectory = file.substr(0, file.find_last_of('/') + 1);

    for (const auto& image : jsonContents["images"])
    {
        const std::string uri = image["uri"];

        // Determine texture type based on URI naming convention
        TextureType type;
        if (uri.find("baseColor") != std::string::npos)
            type = TextureType::DIFFUSE;
        else if (uri.find("metallicRoughness") != std::string::npos)
            type = TextureType::SPECULAR;
        else
        {
            Console::get().warn("[ModelData::loadTextures] Unknown texture type for URI: '" + uri + "'");
            continue;
        }

        // Load the new texture
        std::string texturePath = fileDirectory + uri;
        TextureData textureData(texturePath.c_str(), type);
        loadedTextures.push_back(textureData); // So we can avoid reloading it later
    }
}

std::vector<glm::vec2> ModelData::toVec2(std::vector<float> floatVec)
{
	if (floatVec.size() % 2 != 0) {
        Console::get().error("[ModelData::toVec2] Float vector size is not a multiple of 2: '" + file + "'");
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

std::vector<glm::vec3> ModelData::toVec3(std::vector<float> floatVec)
{
    if (floatVec.size() % 3 != 0) {
        Console::get().error("[ModelData::toVec3] Float vector size is not a multiple of 3: '" + file + "'");
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

std::vector<glm::vec4> ModelData::toVec4(std::vector<float> floatVec)
{
	if (floatVec.size() % 4 != 0) {
        Console::get().error("[ModelData::toVec4] Float vector size is not a multiple of 4: '" + file + "'");
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