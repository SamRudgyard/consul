#pragma once

#include "nlohmann/json.hpp"
#include "graphics/mesh.hpp"
#include "graphics/texture.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <vector>
#include <string>

using json = nlohmann::json;

class IShader;
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
	 * Gets the transformation matrices for each mesh in the model, with the
	 * model's runtime transform applied. Recalculates when needed.
	 * @returns Vector of transformation matrices.
	 */
	std::vector<glm::mat4> getTransformationMatrices();

	/**
	 * Apply a translation to the model's runtime transform.
	 * @param delta Translation to apply.
	 */
	void translate(const glm::vec3& delta);

	/**
	 * Apply a rotation (in radians) around the given axis.
	 * @param angleRadians Rotation angle in radians.
	 * @param axis Axis to rotate around.
	 */
	void rotate(float angleRadians, const glm::vec3& axis);

	/**
	 * Apply a scale to the model's runtime transform.
	 * @param factor Scale factor.
	 */
	void scale(const glm::vec3& factor);

	/**
	 * Set the model's runtime transform matrix directly.
	 * @param transform New transform matrix.
	 */
	void setTransform(const glm::mat4& transform);

	/**
	 * Reset the model's runtime transform to identity.
	 */
	void resetTransform();

	/**
	 * Get the model's runtime transform matrix.
	 * @returns Current runtime transform matrix.
	 */
	const glm::mat4& getTransform() const { return modelTransform; }

private:
	// Variables for easy access
	std::string file;
	std::string fileDirectory;
	std::vector<unsigned char> binaryData;
	json jsonContents;

	std::vector<Mesh> meshes;
	std::vector<glm::mat4> initialTransformations;
	std::vector<glm::mat4> combinedTransforms;
	glm::mat4 modelTransform = glm::mat4(1.0f);
	bool recalcTransformation = true;

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
	 * Retreive a texture's full path from a glTF texture index.
	 * @param textureIndex Index into the glTF textures array.
	 * @returns Full texture path.
	 */
	std::string getTexturePathFromUri(unsigned int textureIndex) const;

	/**
	 * Collect textures referenced by a material.
	 * @param materialIndex Index into the glTF materials array.
	 * @returns Vector of textures referenced by the material.
	 */
	std::vector<Texture> getTexturesForMaterial(int materialIndex) const;

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
