#include "core/consul.hpp"
#include "shader_manager.hpp"
#include "core/models/model.hpp"
#include "core/camera/camera.hpp"

int main(int argc, char **argv) {
    Consul consul("Game", false);

    consul.setVSync(true);

    Camera camera(Window::width, Window::height, glm::vec3(0.0f, 0.0f, 2.0f));

    Shader& shader = ShaderManager::loadShader("shaders/basic.vs", "shaders/basic.fs", "basicShader");

    Model model("assets/shiba/scene.gltf");

    while (consul.run()) {
        model.draw(shader, camera, glm::vec3(0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(0.1f));

        camera.handleInputs(1/60.0f);
        camera.updateMatrix(45.0f, 0.1f, 100.0f);

        glfwPollEvents();
        Window::swapBuffers();
    }

    consul.terminate();

    return 0;
}