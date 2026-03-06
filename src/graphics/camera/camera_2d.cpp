#include "graphics/camera/camera_2d.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "maths/unit_conversions.hpp"
#include "maths/vectors.hpp"

void Camera2D::sendToShader(const IShader* shader) const {
    glm::vec3 position3D = glm::vec3(position, 0.0f);

    shader->setUniformVec3("cameraPosition", position3D);

    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, position3D);
    transform = glm::rotate(transform, rotationDeg*DEG_TO_RAD, Z_AXIS);
    transform = glm::scale(transform, glm::vec3(zoom, zoom, 1.0f));

    shader->setUniformMat4("cameraMatrix", transform);
}

void Camera2D::handleInputs(float deltaTime) {
    InputSystem& input = EngineContext::get()->inputSystem;

    if (input.isKeyDown(KeyboardKey::KEY_W)) {
        position.y -= 5.0f*deltaTime;
    }
    if (input.isKeyDown(KeyboardKey::KEY_A)) {
        position.x += 5.0f*deltaTime;
    }
    if (input.isKeyDown(KeyboardKey::KEY_S)) {
        position.y += 5.0f*deltaTime;
    }
    if (input.isKeyDown(KeyboardKey::KEY_D)) {
        position.x -= 5.0f*deltaTime;
    }
    if (input.getMouseScrollOffset().y != 0.0f) {
        zoom += 0.1f*input.getMouseScrollOffset().y;
        zoom = std::max(std::min(zoom, 1.0f), 0.1f);
    }
}