#include "model.hpp"

#include <utility>

#include "core/console/console.hpp"

void Model::addMesh(
    const std::shared_ptr<Mesh>& mesh,
    const std::shared_ptr<Material>& material,
    const glm::mat4& localTransform
)
{
    if (!mesh) {
        Console::get().error("[Model::addMesh] Mesh is null!");
        return;
    }
    if (!material) {
        Console::get().error("[Model::addMesh] Material is null!");
        return;
    }

    meshes.push_back(mesh);
    materials.push_back(material);
    localTransforms.push_back(localTransform);
}
