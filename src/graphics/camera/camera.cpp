#include "graphics/camera/camera.hpp"
#include "maths/constants.hpp"
#include "core/engine_context.hpp"

Camera::Camera(glm::vec3 position)
    : position(position)
{
}

void Camera::updateMatrix(float FOVdeg, float nearPlane, float farPlane) {
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);

    view = glm::lookAt(position, position + orientation, up);

    // Use FoV angle from larger dimension, see https://stackoverflow.com/questions/26997631/limiting-fov-both-horizontally-and-vertically
    WindowConfig& config = EngineContext::get()->windowConfig;
    float tanFov = tan(0.5f*FOVdeg*DEG_TO_RAD);
    float aspRat = (float)config.windowSize.x / (float)config.windowSize.y;

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

    cameraMatrix = projection*view;
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

    // Mouse controls
    if (input.isMouseButtonDown(MouseButton::BUTTON_RIGHT)) {
        input.setMouseVisibility(false);

        WindowConfig& config = EngineContext::get()->windowConfig;
        float width = (float)config.windowSize.x;
        float height = (float)config.windowSize.y;

        if (input.isMouseButtonPressed(MouseButton::BUTTON_RIGHT)) {
            
            input.setMousePosition(glm::vec2(width/2, height/2));
        }

        glm::vec2 mousePos = input.getMousePosition();
        float rotationX = sensitivity*(float)(mousePos.y - height/2)/height;
        float rotationY = sensitivity*(float)(mousePos.x - width/2)/width;

        glm::vec3 newOrientation = glm::rotate(orientation, glm::radians(-rotationX), glm::normalize(glm::cross(orientation, up)));
        if (!((glm::angle(newOrientation, up) <= glm::radians(5.0f)) || (glm::angle(newOrientation, -up) <= glm::radians(5.0f)))) {
            orientation = newOrientation;
        }

        orientation = glm::rotate(orientation, glm::radians(-rotationY), up);

        // Set mouse position to the centre of the screen
        input.setMousePosition(glm::vec2(width/2, height/2));
    } else {
        input.setMouseVisibility(true);
    }
}

void Camera::useCameraMatrix(Shader& shader, const char* uniform)
{
	// Exports camera matrix
	// glUniformMatrix4fv(glGetUniformLocation(shader.id, uniform), 1, GL_FALSE, glm::value_ptr(cameraMatrix));
}