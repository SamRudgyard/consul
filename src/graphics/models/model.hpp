#pragma once

#include "glm/glm.hpp"

#include <memory>
#include <string>
#include <vector>

class Mesh;
class Material;

struct ModelPrimitive
{
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<Material> material;
	glm::mat4 localTransform = glm::mat4(1.0f);
};

class Model
{
public:
	Model() = default;

	/**
	 * Gets the render primitives in this model.
	 * @returns Vector of model primitives.
	 */
	const std::vector<ModelPrimitive>& getPrimitives() const { return primitives; }

	/**
	 * Add a render primitive to the model.
	 * @param mesh Mesh to add.
	 * @param material Material to use when rendering the mesh.
	 * @param localTransform Transform from mesh-local space to model space.
	 */
	void addPrimitive(
		std::shared_ptr<Mesh> mesh,
		std::shared_ptr<Material> material,
		const glm::mat4& localTransform = glm::mat4(1.0f)
	);

	/**
	 * Get the file path of this model, if it was loaded from a file.
	 * @returns Full path to the model file, or an empty string if the model was not imported from a file.
	 */
	const std::string& getFilePath() const { return fileFullPath; }

	void setFilePath(const std::string& filePath) { fileFullPath = filePath; }

private:
	std::string fileFullPath = "";
	std::vector<ModelPrimitive> primitives;
};
