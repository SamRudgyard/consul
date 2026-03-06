#pragma once

#include "core/engine_context.hpp"
#include "graphics/shaders/shader.hpp"

enum class ProjectionType
{
    PERSPECTIVE,
    ORTHOGRAPHIC
};

class Camera {
public:
    ProjectionType getProjectionType() const { return projectionType; }

    /**
     * Set the projection type (see `ProjectionType`).
     * @param projectionType New projection type.
     */
    void setProjectionType(ProjectionType projectionType) { this->projectionType = projectionType; }

    virtual void handleInputs(float deltaTime) = 0;

    virtual void sendToShader(const IShader* shader) const = 0;

protected:
    EngineContext* context = EngineContext::get();
    ProjectionType projectionType;
};
