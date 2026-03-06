#pragma once

#ifndef GLM_ENABLE_EXPERIMENTAL
    #define GLM_ENABLE_EXPERIMENTAL
#endif

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtx/vector_angle.hpp"

#include "graphics/camera/camera.hpp"

class IShader;

class Camera3D : public Camera {
public:
    float speed = 0.1f;
    float sensitivity = 0.1f;

    Camera3D() = default;

    /**
     * A camera with given viewport width/height and world position.
     * @param position World space position of the camera.
     */
    Camera3D(glm::vec3 position);

    /**
     * Set the world space position of the camera.
     * @param position New world space position of the camera.
     */
    void setPosition(const glm::vec3& position);

    /**
     * Set the field of view in degrees.
     * @param FOVdeg Field of view angle in degrees.
     */
    void setFieldOfView(float FOVdeg);

    /**
     * Set the near clipping plane distance.
     * @param near Near clipping plane distance.
     */
    void setNearPlane(float near);

    /**
     * Set the far clipping plane distance.
     * @param far Far clipping plane distance.
     */
    void setFarPlane(float far);

    /**
     * Set an orthographic projection volume.
     * Coordinates are in world units.
     */
    void setOrthographic(float left, float right, float bottom, float top);
    
    /**
     * Handle basic WASD + space/ctrl movement and RMB-look input.
     * @param deltaTime Time (s) since last frame.
     */
    void handleInputs(float deltaTime) override;

    /**
     * Sends the camera matrix (projection * view) to the given shader uniform.
     * @param shader  Shader to send the matrix to.
     */
    void sendToShader(const IShader* shader) const override;
private:
    glm::vec3 position;
    float nearPlane = 0.1f;
    float farPlane = 100.0f;
    float left = -1.0f;
    float right = 1.0f;
    float bottom = -1.0f;
    float top = 1.0f;
    glm::mat4 viewMatrix = glm::mat4(1.0f);
    glm::mat4 projectionMatrix = glm::mat4(1.0f);

    // Perspective projection specific
    float FOVdeg = 45.0f;
    glm::vec3 orientation = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    /**
     * Update the projection matrix based on current FOV, near and far planes.
    */
    void updateProjectionMatrix();

    /**
     * Update the view matrix based on current position and orientation.
    */
    void updateViewMatrix();
};
