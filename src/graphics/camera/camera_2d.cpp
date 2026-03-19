#include "graphics/camera/camera_2d.hpp"

#include <algorithm>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "maths/unit_conversions.hpp"
#include "maths/vectors.hpp"

void Camera2D::sendToShader(const Shader* shader) const {
    glm::vec3 position3D = glm::vec3(position, 0.0f);
    const glm::vec2 framebufferSize = context->window.framebufferSize;
    const float safeHeight = std::max(framebufferSize.y, 1.0f);
    const float aspectRatio = framebufferSize.x / safeHeight;
    const float halfHeight = 0.5f * viewHeight / zoom;
    const float halfWidth = halfHeight * aspectRatio;

    shader->setUniformVec3("cameraPosition", position3D);

    glm::mat4 projection = glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, -1.0f, 1.0f);

    glm::mat4 view = glm::mat4(1.0f);
    view = glm::rotate(view, -rotationDeg*DEG_TO_RAD, Z_AXIS);
    view = glm::translate(view, -position3D);

    shader->setUniformMat4("cameraMatrix", projection * view);
}

void Camera2D::handleInputs(double deltaTime) {
    InputSystem& input = EngineContext::get()->inputSystem;

    if (input.isKeyDown(KeyboardKey::KEY_W)) {
        position.y += 5.0f*deltaTime;
    }
    if (input.isKeyDown(KeyboardKey::KEY_A)) {
        position.x -= 5.0f*deltaTime;
    }
    if (input.isKeyDown(KeyboardKey::KEY_S)) {
        position.y -= 5.0f*deltaTime;
    }
    if (input.isKeyDown(KeyboardKey::KEY_D)) {
        position.x += 5.0f*deltaTime;
    }
    if (input.getMouseScrollOffset().y != 0.0f) {
        zoom += 0.1f*input.getMouseScrollOffset().y;
        zoom = std::max(std::min(zoom, 1.0f), 0.1f);
    }
}
