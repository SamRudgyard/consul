#pragma once

#include "glm/glm.hpp"

#include <memory>
#include <string>
#include <vector>

class Mesh;
class Material;

class Model
{
public:
	Model() = default;

	/**
	 * Gets the meshes associated with this model.
	 * @return Vector of meshes.
	 */
	const std::vector<std::shared_ptr<Mesh>>& getMeshes() const { return meshes; }

	/**
	 * Gets the materials associated with this model.
	 * @return Vector of materials.
	 */
	const std::vector<std::shared_ptr<Material>>& getMaterials() const { return materials; }

	/**
	 * Gets the local transforms associated with this model.
	 * @return Vector of local transforms.
	 */
	const std::vector<glm::mat4>& getLocalTransforms() const { return localTransforms; }

	/**
	 * Add a mesh to the model.
	 * @param mesh Mesh to add.
	 * @param material Material to use when rendering the mesh.
	 * @param localTransform Local transform of the mesh, with respect to the model.
	 */
	void addMesh(
		const std::shared_ptr<Mesh>& mesh,
		const std::shared_ptr<Material>& material,
		const glm::mat4& localTransform = glm::mat4(1.0f)
	);

private:
	std::vector<std::shared_ptr<Mesh>> meshes;
	std::vector<std::shared_ptr<Material>> materials;
	std::vector<glm::mat4> localTransforms;
};
