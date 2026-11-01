#include "model.hpp"

#include <utility>

#include "core/console/console.hpp"

void Model::addPrimitive(
    const std::shared_ptr<Mesh>& mesh,
    const std::shared_ptr<Material>& material,
    const glm::mat4& localTransform
)
{
    if (!mesh) {
        Console::get().error("[Model::addPrimitive] Mesh is null!");
        return;
    }
    if (!material) {
        Console::get().error("[Model::addPrimitive] Material is null!");
        return;
    }

    ModelPrimitive primitive;
    primitive.mesh = mesh;
    primitive.material = material;
    primitive.localTransform = localTransform;
    primitives.push_back(primitive);
}
