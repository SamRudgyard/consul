#include "graphics/camera/camera.hpp"
#include "maths/constants.hpp"
#include "core/engine_context.hpp"
#include "graphics/shaders/shader.hpp"

Camera::Camera(glm::vec3 position)
    : position(position)
{
    updateProjectionMatrix();
    updateViewMatrix();
}

void Camera::setFieldOfView(float FOVdeg)
{
    this->FOVdeg = FOVdeg;
    updateProjectionMatrix();
}

void Camera::setNearPlane(float nearPlane)
{
    this->nearPlane = nearPlane;
    updateProjectionMatrix();
}

void Camera::setFarPlane(float farPlane)
{
    this->farPlane = farPlane;
    updateProjectionMatrix();
}

void Camera::updateProjectionMatrix()
{
    glm::mat4 projection = glm::mat4(1.0f);

    // Use FoV angle from larger dimension, see https://stackoverflow.com/questions/26997631/limiting-fov-both-horizontally-and-vertically
    Window& window = EngineContext::get()->window;
    float tanFov = tan(0.5f*FOVdeg*DEG_TO_RAD);
    float aspRat = (float)window.windowSize.x / (float)window.windowSize.y;

    projection[0][0] = 1.0f / (aspRat * tanFov);
    projection[0][1] = 0.0f;
    projection[0][2] = 0.0f;
    projection[0][3] = 0.0f;

    projection[1][0] = 0.0f;
    projection[1][1] = 1.0f / tanFov;
    projection[1][2] = 0.0f;
    projection[1][3] = 0.0f;

    projection[2][0] = 0.0f;
    projection[2][1] = 0.0f;
    projection[2][2] = (nearPlane + farPlane) / (nearPlane - farPlane);
    projection[2][3] = -1.0f;

    projection[3][0] = 0.0f;
    projection[3][1] = 0.0f;
    projection[3][2] = 2.0f * nearPlane * farPlane / (nearPlane - farPlane);
    projection[3][3] = 0.0f;

    projectionMatrix = projection;
}

void Camera::updateViewMatrix()
{
    glm::mat4 view = glm::mat4(1.0f);
    view = glm::lookAt(position, position + orientation, up);
    viewMatrix = view;
}

void Camera::handleInputs(float deltaTime) {
    InputSystem& input = EngineContext::get()->inputSystem;

    if (input.isKeyDown(KeyboardKey::KEY_W)) {
        position += speed*orientation*deltaTime;
    }
    if (input.isKeyDown(KeyboardKey::KEY_A)) {
        position -= speed*glm::normalize(glm::cross(orientation, up))*deltaTime;
    }
    if (input.isKeyDown(KeyboardKey::KEY_S)) {
        position -= speed*orientation*deltaTime;
    }
    if (input.isKeyDown(KeyboardKey::KEY_D)) {
        position += speed*glm::normalize(glm::cross(orientation, up))*deltaTime;
    }
    if (input.isKeyDown(KeyboardKey::KEY_SPACE)) {
        position += speed*up*deltaTime;
    }
    if (input.isKeyDown(KeyboardKey::KEY_LEFT_CONTROL)) {
        position -= speed*up*deltaTime;
    }
    if (input.isKeyDown(KeyboardKey::KEY_LEFT_SHIFT)) {
        speed = 10.0f;
    }
    else if (!input.isKeyDown(KeyboardKey::KEY_LEFT_SHIFT)) {
        speed = 5.0f;
    }

    if (input.isMouseButtonDown(MouseButton::BUTTON_RIGHT)) {
        input.setMouseVisibility(false);

        Window& window = EngineContext::get()->window;
        float width = (float)window.windowSize.x;
        float height = (float)window.windowSize.y;

        glm::vec2 mousePos = input.getMousePosition();
        glm::vec2 prevMousePos = input.getPreviousMousePosition();
        float rotationX = sensitivity*(float)(mousePos.y - prevMousePos.y);
        float rotationY = sensitivity*(float)(mousePos.x - prevMousePos.x);

        glm::vec3 xRotation = glm::rotate(orientation, glm::radians(-rotationX), glm::normalize(glm::cross(orientation, up)));
        orientation = glm::rotate(xRotation, glm::radians(-rotationY), up);
    } else {
        input.setMouseVisibility(true);
    }

    updateViewMatrix();
    updateProjectionMatrix();
}

void Camera::sendToShader(const IShader* shader) const
{
    shader->setUniformVec3("cameraPosition", position);
    shader->setUniformMat4("cameraMatrix", projectionMatrix * viewMatrix);
}