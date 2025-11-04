#include "core/consul.hpp"
#include "shader_manager.hpp"
#include "core/models/model.hpp"
#include "core/camera/camera.hpp"

int main(int argc, char **argv) {
    Consul consul("Game", false);

    consul.VSync(true);

    Camera camera(Window::width, Window::height, glm::vec3(0.0f, 0.0f, 2.0f));

    Shader& shader = ShaderManager::LoadShader("shaders/basic.vs", "shaders/basic.fs", "basicShader");

    Model model("assets/shiba/scene.gltf");

    while (consul.Run()) {
        model.Draw(shader, camera);

        camera.HandleInputs(1/60.0f);
        camera.UpdateMatrix(45.0f, 0.1f, 100.0f);

        glfwPollEvents();
        Window::SwapBuffers();
    }

    consul.Terminate();

    return 0;
}