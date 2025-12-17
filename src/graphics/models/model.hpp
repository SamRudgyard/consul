#pragma once

#include "nlohmann/json.hpp"
#include "graphics/mesh/mesh.hpp"
#include "graphics/textures/texture.hpp"
#include "glad/glad.h"
#include "glm/glm.hpp"

#include <vector>
#include <string>

using json = nlohmann::json;

class IShader;
class Camera;
class RenderableTexture;

class Model
{
public:
	/**
	 * Load the given glTF model.
	 * @param file Path to the .gltf model file.
	 */
	Model(const char* file);

	/*
	 * Gets the meshes loaded from this model.
	 * @returns Vector of meshes. 
	 */
	std::vector<Mesh> getMeshes() const { return meshes; }

	/**
	 * Gets the transformation matrices for each mesh in the model.
	 * @returns Vector of transformation matrices.
	 */
	std::vector<glm::mat4> getTransformationMatrices() const { return transformationMatrices; }

private:
	// Variables for easy access
	std::string file;
	std::vector<unsigned char> binaryData;
	json jsonContents;

	std::vector<Mesh> meshes;
	std::vector<Texture> loadedTextures;
	std::vector<glm::mat4> transformationMatrices;

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
	 */
	void loadTextures();

	/**
	 * Convert a float array to a vec2 array.
	 * @param floatVec Vector of floats.
	 * @returns Vector of vec2.
	 */
	std::vector<glm::vec2> toVec2(const std::vector<float> floatVec);

	/**
	 * Convert a float array to a vec3 array.
	 * @param floatVec Vector of floats.
	 * @returns Vector of vec3.
	 */
	std::vector<glm::vec3> toVec3(const std::vector<float> floatVec);

	/**
	 * Convert a float array to a vec4 array.
	 * @param floatVec Vector of floats.
	 * @returns Vector of vec4.
	 */
	std::vector<glm::vec4> toVec4(const std::vector<float> floatVec);
};