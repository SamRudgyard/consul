#pragma once

#include "nlohmann/json.hpp"
#include "core/models/mesh.hpp"
#include "glad/glad.h"
#include "glm/glm.hpp"

#include <vector>
#include <string>

using json = nlohmann::json;

class Shader;
class Camera;
class Texture;

class Model
{
public:
	// Loads in a model from a file and stores tha information in 'data', 'JSON', and 'file'
	Model(const char* file);

	void Draw
	(
		Shader& shader,
		Camera& camera,
		glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
		glm::vec3 scale = glm::vec3(0.1f, 0.1f, 0.1f)
	);

private:
	// Variables for easy access
	std::string file;
	std::vector<unsigned char> binaryData;
	json jsonContents;

	// All the meshes and transformations
	std::vector<Mesh> meshes;
	std::vector<glm::mat4> transformationMatrices;

	std::vector<std::string> loadedTextureFiles;
    std::vector<Texture> loadedTextures;

	// Loads a single mesh by its index
	void loadMesh(unsigned int iMesh);

	void traverseNode(unsigned int nextNode, glm::mat4 parentTransMatrix = glm::mat4(1.0f));

	// Interprets the binary data into floats, indices, and textures
	std::vector<float> readAccessorFloats(json accessor);
	std::vector<unsigned int> readAccessorIndices(json accessor);
	std::vector<Texture> loadTextures();

	// Assembles all the floats into vertices
	std::vector<Vertex> assembleVertices(std::vector<glm::vec3> positions, std::vector<glm::vec3> normals, std::vector<glm::vec2> texUVs);

	std::vector<glm::vec2> toVec2(std::vector<float> floatVec);
	std::vector<glm::vec3> toVec3(std::vector<float> floatVec);
	std::vector<glm::vec4> toVec4(std::vector<float> floatVec);
};