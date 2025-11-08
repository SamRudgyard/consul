#pragma once

#ifndef GLM_ENABLE_EXPERIMENTAL
    #define GLM_ENABLE_EXPERIMENTAL
#endif

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtx/vector_angle.hpp"

#include "shader.hpp"

class Camera {
public:
    glm::vec3 position;
    glm::vec3 orientation = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::mat4 cameraMatrix = glm::mat4(1.0f);
    int width, height;
    float speed = 0.1f;
    float sensitivity = 100.0f;
    bool isFirstClick = true;

    Camera(int width, int height, glm::vec3 position);
    void UpdateMatrix(float FOVdeg, float nearPlane, float farPlane);
    void HandleInputs(float deltaTime);
    void Matrix(Shader& shader, const char* uniform);
};