#pragma once

#include "nlohmann/json.hpp"
#include "graphics/material/material.hpp"
#include "graphics/mesh/mesh.hpp"
#include "graphics/texture/texture.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <vector>
#include <string>
#include <memory>

#include "core/project/asset_types.hpp"

using json = nlohmann::json;

struct Model
{
	std::vector<glm::mat4> transforms;
	std::vector<AssetID> meshIDs;
};

class IShader;
class Model
{
public:
	Model() = default;

	/**
	 * Load the given glTF model.
	 * @param file Path to the .gltf model file.
	 */
	Model(const char* file);

	/**
	 * Create a model from a single mesh.
	 * @param mesh Mesh to initialize the model with.
	 */
	Model(Mesh mesh);

	/**
	 * Gets the meshes loaded from this model.
	 * @returns Vector of meshes. 
	 */
	std::vector<Mesh>& getMeshes() { return meshes; }
	const std::vector<Mesh>& getMeshes() const { return meshes; }

	/**
	 * Add a mesh to the model, with an optional initial transform.
	 * @param mesh Mesh to add.
	 * @param initialTransform Initial transform matrix. Defaults to the identity matrix (i.e. no initial transform).
	 */
	void addMesh(Mesh mesh, const glm::mat4& initialTransform = glm::mat4(1.0f));

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

	/**
	 * Get the file path of this model, if it was loaded from a file.
	 * @returns Full path to the model file, or an empty string if the model was not loaded from a file.
	 */
	const std::string& getFilePath() const { return fileFullPath; }

private:
	std::string fileFullPath = "";
	std::string fileDirectory = "";
	std::vector<unsigned char> binaryData;
	json jsonContents;

	std::vector<Mesh> meshes;
	std::vector<std::shared_ptr<Material>> materials;
	std::vector<glm::mat4> initialTransformations;
	std::vector<glm::mat4> combinedTransforms;
	glm::mat4 modelTransform = glm::mat4(1.0f);
	bool recalcTransformation = true;

	/**
	 * Load a single mesh.
	 * @param iMesh Index of the mesh, as found in the glTF file.
	 */
	void loadMesh(unsigned int iMesh, const glm::mat4& initialTransform);
};
