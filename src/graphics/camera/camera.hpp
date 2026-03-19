#pragma once

#include "core/engine_context.hpp"
#include "glm/glm.hpp"

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
    virtual void setProjectionType(ProjectionType projectionType) { this->projectionType = projectionType; }

    virtual void handleInputs(double deltaTime) = 0;

    virtual glm::vec3 getPosition() const = 0;
    virtual glm::mat4 getCameraMatrix() const = 0;

protected:
    EngineContext* context = EngineContext::get();
    ProjectionType projectionType = ProjectionType::PERSPECTIVE;
};
