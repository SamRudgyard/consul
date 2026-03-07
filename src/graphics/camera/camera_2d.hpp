#pragma once

#include "glm/glm.hpp"

#include "graphics/camera/camera.hpp"
#include "graphics/shaders/shader.hpp"

class Camera2D : public Camera {
public:
    Camera2D() { setProjectionType(ProjectionType::ORTHOGRAPHIC); }

     /**
     * Set the world space position of the camera.
     * @param position New world space position of the camera.
     */
    void setPosition(const glm::vec2& position) { this->position = position; }

    /**
    * Set the rotation of the camera in degrees.
    * @param rotation Rotation angle in degrees.
    */
    void setRotation(float rotation) { this->rotationDeg = rotation; }

    /**
     * Set the zoom level of the camera (1.0f is default, < 1.0f zooms in, > 1.0f zooms out).
     * @param zoom New zoom level.
     */
    void setZoom(float zoom) { this->zoom = zoom; }

    void sendToShader(const IShader* shader) const override;

    void handleInputs(double deltaTime) override;

private:
    glm::vec2 position = glm::vec2(0.0f, 0.0f);
    float rotationDeg = 0.0f;
    float zoom = 1.0f;
    float viewHeight = 2.0f;
};
