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
	/**
	 * Load the given glTF model.
	 * @param file Path to the .gltf model file.
	 */
	Model(const char* file);

	/**
	 * Draws the model.
	 * @param shader      Shader program to use for drawing.
	 * @param camera      Camera providing view/projection matrices.
	 * @param translation Additional translation to apply.
	 * @param rotation    Additional rotation to apply.
	 * @param scale       Additional scale to apply.
	 */
	void draw
	(
		Shader& shader,
		Camera& camera,
		glm::vec3 translation = glm::vec3(0.0f),
		glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
		glm::vec3 scale = glm::vec3(1.0f)
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

	/**
	 * Load a single mesh.
	 * @param iMesh Index of the mesh, as found in the glTF file.
	 */
	void loadMesh(unsigned int iMesh);

	/**
	 * Traverse a node within the glTF file recursively to collect meshes and construct transforms.
	 * @param nextNode            Index of the node to traverse.
	 * @param parentTransMatrix   Transformation matrix of the parent node.
	 */
	void traverseNode(unsigned int nextNode, glm::mat4 parentTransMatrix = glm::mat4(1.0f));

	/**
	 * Read float attribute data referenced by an accessor.
	 * @param accessor JSON accessor object.
	 * @returns Vector of floats read from the accessor.
	 */
	std::vector<float> readAccessorFloats(json accessor);

	/**
	 * Read index attribute data referenced by an accessor.
	 * @param accessor JSON accessor object.
	 * @returns Vector of unsigned ints read from the accessor.
	 */
	std::vector<unsigned int> readAccessorIndices(json accessor);

	/**
	 * Load textures used by the model.
	 * @returns Vector of loaded textures.
	 */
	std::vector<Texture> loadTextures();

	/**
	 * Assemble separate position, normal, and UV arrays into Vertex arrays.
	 * @param positions Vertex positions.
	 * @param normals   Vertex normals.
	 * @param texUVs    Vertex texture coordinates.
	 * @returns Vector of assembled Vertex structures.
	 */
	std::vector<Vertex> assembleVertices(
		std::vector<glm::vec3> positions,
		std::vector<glm::vec3> normals,
		std::vector<glm::vec2> texUVs
	);

	/**
	 * Convert a float array to a vec2 array.
	 * @param floatVec Vector of floats.
	 * @returns Vector of vec2.
	 */
	std::vector<glm::vec2> toVec2(std::vector<float> floatVec);

	/**
	 * Convert a float array to a vec3 array.
	 * @param floatVec Vector of floats.
	 * @returns Vector of vec3.
	 */
	std::vector<glm::vec3> toVec3(std::vector<float> floatVec);

	/**
	 * Convert a float array to a vec4 array.
	 * @param floatVec Vector of floats.
	 * @returns Vector of vec4.
	 */
	std::vector<glm::vec4> toVec4(std::vector<float> floatVec);
};