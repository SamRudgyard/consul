#include "core/models/model.hpp"
#include "shader.hpp"
#include "core/camera/camera.hpp"
#include "core/models/texture.hpp"
#include "core/console/console.hpp"
#include "utils.hpp"

#include <algorithm>

Model::Model(const char* modelPath)
    : file(std::string(modelPath))
{
	// Make a JSON object
	std::string text = ReadFile(modelPath);
	jsonContents = json::parse(text);

    if (GetFileExtension(modelPath) != ".gltf") {
        Console::Get().Error("Unsupported model format in file: '" + file + "'. Only .gltf files are supported.");
    }

    // The uri (unique resource identifier) of the binary data
	std::string uri = jsonContents["buffers"][0]["uri"];

	std::string fileDirectory = file.substr(0, file.find_last_of('/') + 1);
	std::string binaryContents = ReadFile((fileDirectory + uri).c_str());
    binaryData = std::vector<unsigned char>(binaryContents.begin(), binaryContents.end());

	// Traverse all nodes
	traverseNode(0);
}

void Model::Draw(Shader& shader, Camera& camera, glm::vec3 translation, glm::quat rotation, glm::vec3 scale)
{
	for (unsigned int im = 0; im < meshes.size(); im++)
	{
		meshes[im].Mesh::Draw(shader, camera, transformationMatrices[im]);
	}
}

void Model::loadMesh(unsigned int iMesh)
{
	// Get all accessor indices
    json primitives = jsonContents["meshes"][iMesh]["primitives"];
    if (primitives.size() < 1) {
        Console::Get().Error("[Model::LoadMesh] Mesh " + std::to_string(iMesh) + " contains no primitives.");
    }
    json attributes = primitives[0]["attributes"];
	unsigned int positionAccessorIndex = attributes["POSITION"];
	unsigned int normalAccessorIndex = attributes["NORMAL"];
	unsigned int texCoordAccessorIndex = attributes["TEXCOORD_0"];
	unsigned int indexAccessorIndex = primitives[0]["indices"];

	// Use accessor indices to get all vertices components
	std::vector<float> positionFloats = readAccessorFloats(jsonContents["accessors"][positionAccessorIndex]);
	std::vector<glm::vec3> positions = toVec3(positionFloats);
	std::vector<float> normalFloats = readAccessorFloats(jsonContents["accessors"][normalAccessorIndex]);
	std::vector<glm::vec3> normals = toVec3(normalFloats);
	std::vector<float> textureFloats = readAccessorFloats(jsonContents["accessors"][texCoordAccessorIndex]);
	std::vector<glm::vec2> textureUVs = toVec2(textureFloats);

	// Combine all the vertex components and also get the indices and textures
	std::vector<Vertex> vertices = assembleVertices(positions, normals, textureUVs);
	std::vector<GLuint> indices = readAccessorIndices(jsonContents["accessors"][indexAccessorIndex]);
	std::vector<Texture> textures = loadTextures();

	// Combine the vertices, indices, and textures into a mesh
	meshes.push_back(Mesh(vertices, indices, textures));
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
        Console::Get().Error("[Model::readAccessorFloats] Invalid component type '" + componentType + "' in accessor (not SCALAR, VEC2, VEC3, or VEC4): '" + file + "'");
    }
    unsigned int bytesPerComponent = it->second;

	unsigned int viewByteOffset = jsonContents["bufferViews"][bufferViewIndex].value("byteOffset", 0);

	unsigned int dataStart = byteOffset + viewByteOffset;
    unsigned int nFloats = bytesPerComponent*nComponents;
	unsigned int dataEnd = dataStart + sizeof(float)*nFloats;

    if (dataEnd > binaryData.size()) {
        Console::Get().Error("[Model::readAccessorFloats] Attempting to read out-of-bounds data: '" + file + "'");
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
            Console::Get().Error("[Model::readAccessorIndices] GL_BYTE (5120) is not supported for index accessors: '" + file + "'");
            break;
        case GL_UNSIGNED_BYTE: // 5121
            Console::Get().Error("[Model::readAccessorIndices] GL_UNSIGNED_BYTE (5121) is not supported for index accessors: '" + file + "'");
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
            Console::Get().Error("[Model::readAccessorIndices] GL_FLOAT (5126) is not supported for index accessors: '" + file + "'");
            break;
        default:
            Console::Get().Error("[Model::readAccessorIndices] Invalid component type '" + std::to_string(componentType) + "' in accessor for indices: '" + file + "'");
            break;
    }

    for (unsigned int ic = 0; ic < nComponents; ic++) {
        unsigned int index = 0;
        std::memcpy(&index, binaryData.data() + beginningOfData + ic*nBytesPerComponent, nBytesPerComponent);
        indices.push_back(index);
    }

	return indices;
}

std::vector<Texture> Model::loadTextures()
{
	std::vector<Texture> textures;

	std::string fileDirectory = file.substr(0, file.find_last_of('/') + 1);

	for (unsigned int iImage = 0; iImage < jsonContents["images"].size(); iImage++)
	{
		// Unique resource identifier of the texture, relative to the model file
		std::string uri = jsonContents["images"][iImage]["uri"];

        if (std::find(loadedTextureFiles.begin(), loadedTextureFiles.end(), uri) == loadedTextureFiles.end())
        {
            // Texture has not been loaded yet, so load it
            if (uri.find("baseColor") != std::string::npos)
            {
                Texture diffuseTexture = Texture((fileDirectory + uri).c_str(), TextureType::DIFFUSE, (GLuint)loadedTextureFiles.size());
                loadedTextures.push_back(diffuseTexture);
                loadedTextureFiles.push_back(uri);
            }
            else if (uri.find("metallicRoughness") != std::string::npos)
            {
                Texture specularTexture = Texture((fileDirectory + uri).c_str(), TextureType::SPECULAR, (GLuint)loadedTextureFiles.size());
                loadedTextures.push_back(specularTexture);
                loadedTextureFiles.push_back(uri);
            }
        }
        else
        {
            // Texture has already been loaded, so just reuse it
            unsigned int iTexture = std::find(loadedTextureFiles.begin(), loadedTextureFiles.end(), uri) - loadedTextureFiles.begin();
            textures.push_back(loadedTextures[iTexture]);
        }
	}

	return textures;
}

std::vector<Vertex> Model::assembleVertices
(
	std::vector<glm::vec3> positions,
	std::vector<glm::vec3> normals,
	std::vector<glm::vec2> texUVs
)
{
	std::vector<Vertex> vertices;
	for (int i = 0; i < positions.size(); i++)
	{
		vertices.push_back
		(
			Vertex
			{
				positions[i],
				normals[i],
				glm::vec3(1.0f, 1.0f, 1.0f),
				texUVs[i]
			}
		);
	}
	return vertices;
}

std::vector<glm::vec2> Model::toVec2(std::vector<float> floatVec)
{
	if (floatVec.size() % 2 != 0) {
        Console::Get().Error("[Model::toVec2] Float vector size is not a multiple of 2: '" + file + "'");
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
        Console::Get().Error("[Model::toVec3] Float vector size is not a multiple of 3: '" + file + "'");
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
        Console::Get().Error("[Model::toVec4] Float vector size is not a multiple of 4: '" + file + "'");
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