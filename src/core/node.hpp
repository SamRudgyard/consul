#pragma once

#include <functional>
#include <memory>
#include <type_traits>
#include <utility>
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

    /**
     * Create a child node of type `T` with the given constructor arguments, and add it to this node's children.
      * @tparam T Type of the child node (must inherit from `Node`).
      * @param args Arguments to forward to `T`'s constructor.
      * @return Pointer to the newly created child node.
     */
    template <typename T, typename... Args>
    T* createChild(Args&&... args)
    {
        static_assert(std::is_base_of<Node, T>::value, "T must inherit from Node");

        std::unique_ptr<T> child = std::make_unique<T>(std::forward<Args>(args)...);
        T* childPtr = child.get();
        children.emplace_back(std::move(child));
        return childPtr;
    }

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
    virtual void onUpdate(double deltaTime) {}
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
