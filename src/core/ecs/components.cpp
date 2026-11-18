#include "core/ecs/components.hpp"

#include <glm/gtc/matrix_transform.hpp>

glm::mat4 Camera::getProjectionMatrix(float framebufferAspectRatio) const
{
    if (projectionType == ProjectionType::ORTHOGRAPHIC) {
        const float halfHeight = orthographicHeight * 0.5f;
        const float halfWidth = halfHeight * framebufferAspectRatio;
        return glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, nearPlane, farPlane);
    }

    return glm::perspective(glm::radians(fov), framebufferAspectRatio, nearPlane, farPlane);
}

glm::mat4 Camera::getViewMatrix(const Transform& transform) const
{
    glm::mat4 cameraTransform = glm::translate(glm::mat4(1.0f), transform.position);
    cameraTransform = glm::rotate(cameraTransform, transform.rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    cameraTransform = glm::rotate(cameraTransform, transform.rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    cameraTransform = glm::rotate(cameraTransform, transform.rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    return glm::inverse(cameraTransform);
}

glm::mat4 Camera::getCameraMatrix(const Transform& transform, float framebufferAspectRatio) const
{
    return getProjectionMatrix(framebufferAspectRatio) * getViewMatrix(transform);
}
