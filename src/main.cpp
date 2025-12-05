#include "core/consul.hpp"
#include "core/window_config.hpp"
// #include "core/models/model.hpp"
#include "graphics/camera/camera.hpp"

int main(int argc, char **argv) {

    WindowConfig windowConfig;
    windowConfig.title = "Test";
    windowConfig.isMaximised = true;

    Consul consul(windowConfig);

    Camera camera(glm::vec3(0.0f, 0.0f, 2.0f));

    IShader* shader = consul.getRenderer().newShader("shaders/basic.vs", "shaders/basic.fs");

    // ModelData model("assets/shiba/scene.gltf");

    while (consul.run()) {
        // model.draw(shader, camera, glm::vec3(0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(0.1f));

        camera.handleInputs(1/60.0f);
        camera.sendToShader(shader);
    }

    return 0;
}