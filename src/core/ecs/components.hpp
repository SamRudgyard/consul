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

struct CameraComponent
{
    enum class ProjectionType
    {
        PERSPECTIVE,
        ORTHOGRAPHIC
    };

    ProjectionType projectionType = ProjectionType::PERSPECTIVE;
    float fov = 60.0f;
    float aspectRatio = 1.0f;
    float nearPlane = 0.1f;
    float farPlane = 1000.0f;
};
