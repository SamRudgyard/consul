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

    /**
     * A perspective camera with given viewport width/height and world position.
     * @param width    Viewport width in pixels.
     * @param height   Viewport height in pixels.
     * @param position World space position of the camera.
     */
    Camera(int width, int height, glm::vec3 position);

    /**
     * Recompute the camera matrix (projection * view) for the given perspective parameters.
     * @param FOVdeg     Vertical field of view in degrees.
     * @param nearPlane  Near clipping plane distance.
     * @param farPlane   Far clipping plane distance.
     */
    void updateMatrix(float FOVdeg, float nearPlane, float farPlane);
    
    /**
     * Handle basic WASD + space/ctrl movement and RMB-look input.
     * @param deltaTime Time (s) since last frame.
     */
    void handleInputs(float deltaTime);

    /**
     * Upload the cameraMatrix to the shader uniform (mat4).
     * 
     * @param shader  Shader program to set the uniform in.
     * @param uniform Name of the uniform variable in the shader.
     */
    void useCameraMatrix(Shader& shader, const char* uniform);
};