#include "core/node.hpp"
#include "core/profiling/profiler_scope.hpp"
#include "maths/unit_conversions.hpp"
#include "glm/gtc/matrix_transform.hpp"

Node::Node(const glm::mat4& localTransform)
    : localTransform(localTransform) {}

void Node::addChild(std::unique_ptr<Node> child)
{
    if (!child) {
        return;
    }
    children.emplace_back(std::move(child));
}

const std::vector<std::unique_ptr<Node>>& Node::getChildren() const
{
    return children;
}

void Node::setPosition(glm::vec3 pos)
{
    position = pos;
    recalcLocalTransformation();
}

void Node::setRotationDeg(glm::vec3 rotationDeg)
{
    glm::vec3 rotationRad = {DEG_TO_RAD*rotationDeg.x, DEG_TO_RAD*rotationDeg.y, DEG_TO_RAD*rotationDeg.z};
    setRotationRad(rotationRad);
}

void Node::setRotationRad(glm::vec3 rotationRad)
{
    rotation = rotationRad;
    recalcLocalTransformation();
}

const glm::mat4& Node::getLocalTransform() const
{
    return localTransform;
}

const glm::mat4& Node::getWorldTransform() const
{
    return worldTransform;
}

void Node::update(float dt, const glm::mat4& parentTransform)
{
    CONSUL_PROFILE_METHOD();

    onUpdate(dt);

    worldTransform = parentTransform*localTransform;

    for (const auto& child : children) {
        child->update(dt, worldTransform);
    }
}

void Node::render(Renderer& renderer)
{
    CONSUL_PROFILE_METHOD();

    if (!isVisible) {
        return;
    }

    onRender(renderer);

    for (const auto& child : children) {
        child->render(renderer);
    }
}

void Node::recalcLocalTransformation()
{
    // --------
    // Rotation
    // --------
    glm::mat4 Rx = glm::rotate(glm::mat4(1.0f), rotation.x, glm::vec3(1, 0, 0));
    glm::mat4 Ry = glm::rotate(glm::mat4(1.0f), rotation.y, glm::vec3(0, 1, 0));
    glm::mat4 Rz = glm::rotate(glm::mat4(1.0f), rotation.z, glm::vec3(0, 0, 1));

    // Order of rotation matters. Let us rotate via ZYX.
    // => R = Rz * Ry * Rx
    glm::mat4 R = Rz*Ry*Rx;

    // --------
    // Scale
    // --------
    glm::mat4 S(1.0f);
    S[0][0] = scale.x;
    S[1][1] = scale.y;
    S[2][2] = scale.z;

    // --------
    // Translation
    // --------
    glm::mat4 T = glm::translate(glm::mat4(1.0f), position);

    localTransform = T*R*S;
}
