#include "model.hpp"

#include <utility>

void Model::addPrimitive(
    std::shared_ptr<Mesh> mesh,
    std::shared_ptr<Material> material,
    const glm::mat4& localTransform
)
{
    if (!mesh) {
        return;
    }

    primitives.push_back({std::move(mesh), std::move(material), localTransform});
}
