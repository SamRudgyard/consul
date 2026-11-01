#pragma once

#include <memory>
#include <string>
#include <vector>

#include "graphics/mesh/mesh.hpp"
#include "graphics/material/material.hpp"
#include "glm/glm.hpp"

class Mesh;
class Material;

class ModelPrimitive
{
public:
    ModelPrimitive() = default;
    ModelPrimitive(const std::shared_ptr<Mesh>& mesh, const std::shared_ptr<Material>& material, glm::mat4 localTransform = glm::mat4(1.0f))
	 : mesh(mesh), material(material), localTransform(localTransform) {}

    std::shared_ptr<Mesh> getMesh() const { return mesh; };
    std::shared_ptr<Material> getMaterial() const { return material; };
    glm::mat4 getLocalTransform() const { return localTransform; };

private:
    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<Material> material;
    glm::mat4 localTransform = glm::mat4(1.0f);
};
