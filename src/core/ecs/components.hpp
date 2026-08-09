#pragma once

struct Component
{
    virtual ~Component() = default;
};

struct Position : public Component
{
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
};

struct Velocity : public Component
{
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
};

struct Camera : public Component
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