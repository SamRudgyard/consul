#pragma once

#include <memory>

#include <glm/glm.hpp>

class Model;

struct Transform
{
    glm::vec3 position = {0.0f, 0.0f, 0.0f};
    glm::vec3 rotation = {0.0f, 0.0f, 0.0f}; // Radians
    glm::vec3 scale = {1.0f, 1.0f, 1.0f};
};

struct Velocity
{
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
};

struct ModelRenderer
{
    std::shared_ptr<Model> model;
    bool visible = true;
};

struct Camera
{
    enum class ProjectionType
    {
        PERSPECTIVE,
        ORTHOGRAPHIC
    };

    ProjectionType projectionType = ProjectionType::PERSPECTIVE;
    float fov = 60.0f; // Vertical field of view in degrees
    float orthographicHeight = 2.0f; // Vertical view size in world units
    float nearPlane = 0.1f;
    float farPlane = 1000.0f;

    [[nodiscard]] glm::mat4 getProjectionMatrix(float framebufferAspectRatio) const;
    [[nodiscard]] glm::mat4 getViewMatrix(const Transform& transform) const;
    [[nodiscard]] glm::mat4 getCameraMatrix(const Transform& transform, float framebufferAspectRatio) const;
};
