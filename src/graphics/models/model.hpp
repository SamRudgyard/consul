#pragma once

#include "glm/glm.hpp"

#include <memory>
#include <string>
#include <vector>

class Mesh;

class Model
{
public:
	Model() = default;

	/**
	 * Gets the meshes loaded from this model.
	 * @returns Vector of meshes.
	 */
	const std::vector<std::shared_ptr<Mesh>>& getMeshes() const { return meshes; }

	/**
	 * Add a mesh to the model, with an optional initial transform.
	 * @param mesh Mesh to add.
	 * @param initialTransform Initial transform matrix. Defaults to the identity matrix (i.e. no initial transform).
	 */
	void addMesh(std::shared_ptr<Mesh> mesh, const glm::mat4& initialTransform = glm::mat4(1.0f));

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
	 * @returns Full path to the model file, or an empty string if the model was not imported from a file.
	 */
	const std::string& getFilePath() const { return fileFullPath; }

	void setFilePath(const std::string& filePath) { fileFullPath = filePath; }

private:
	std::string fileFullPath = "";
	std::vector<std::shared_ptr<Mesh>> meshes;
	std::vector<glm::mat4> initialTransformations;
	std::vector<glm::mat4> combinedTransforms;
	glm::mat4 modelTransform = glm::mat4(1.0f);
	bool recalcTransformation = true;
};
