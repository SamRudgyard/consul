#include "model.hpp"

#include "glm/gtc/matrix_transform.hpp"

void Model::addMesh(AssetID meshID, const glm::mat4& initialTransform)
{
    if (meshID == INVALID_ASSET_ID) {
        return;
    }

    meshIDs.push_back(meshID);
    initialTransformations.push_back(initialTransform);
    recalcTransformation = true;
}

std::vector<glm::mat4> Model::getTransformationMatrices()
{
    if (!recalcTransformation) {
        return combinedTransforms;
    }

    combinedTransforms.clear();
    combinedTransforms.reserve(initialTransformations.size());
    for (const glm::mat4& initialTransformation : initialTransformations) {
        combinedTransforms.push_back(modelTransform * initialTransformation);
    }
    recalcTransformation = false;

    return combinedTransforms;
}

void Model::translate(const glm::vec3& delta)
{
    modelTransform = glm::translate(modelTransform, delta);
    recalcTransformation = true;
}

void Model::rotate(float angleRadians, const glm::vec3& axis)
{
    modelTransform = glm::rotate(modelTransform, angleRadians, axis);
    recalcTransformation = true;
}

void Model::scale(const glm::vec3& factor)
{
    modelTransform = glm::scale(modelTransform, factor);
    recalcTransformation = true;
}

void Model::setTransform(const glm::mat4& transform)
{
    modelTransform = transform;
    recalcTransformation = true;
}

void Model::resetTransform()
{
    modelTransform = glm::mat4(1.0f);
    recalcTransformation = true;
}
