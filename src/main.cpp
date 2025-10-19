#include "core/consul.hpp"
#include "shader_manager.hpp"

int main(int argc, char **argv) {
    Consul consul("Game", false);

    consul.VSync(true);

    Shader& shader = ShaderManager::LoadShader("shaders/basic.vs", "shaders/basic.fs", "basicShader");

    while (consul.Run()) {
        shader.Use();
    }

    consul.Terminate();

    return 0;
}