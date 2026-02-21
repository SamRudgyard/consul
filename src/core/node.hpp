#pragma once

#include <functional>
#include <memory>
#include <vector>

#include "glm/glm.hpp"

class Renderer;

class Node
{
public:
    using UpdateCallback = std::function<void(Node&, float)>;
    using RenderCallback = std::function<void(Node&, Renderer&)>;

    Node() = default;
    explicit Node(const glm::mat4& localTransform);
    virtual ~Node() = default;

    Node* createChild();
    void addChild(std::unique_ptr<Node> child);
    const std::vector<std::unique_ptr<Node>>& getChildren() const;

    void setPosition(glm::vec3 pos);
    void setRotationDeg(glm::vec3 rotationDeg);
    void setRotationRad(glm::vec3 rotationRad);

    const glm::mat4& getLocalTransform() const;
    const glm::mat4& getWorldTransform() const;

    void update(float dt, const glm::mat4& parentTransform);
    void render(Renderer& renderer);

protected:
    virtual void onUpdate(float dt) {}
    virtual void onRender(Renderer& renderer) {}

private:
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);

    glm::mat4 localTransform = glm::mat4(1.0f);
    glm::mat4 worldTransform = glm::mat4(1.0f);
    std::vector<std::unique_ptr<Node>> children;
    bool isVisible = true;

    void recalcLocalTransformation();
};
