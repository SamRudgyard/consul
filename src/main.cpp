#include "core/consul.hpp"
#include "core/window_config.hpp"
#include "graphics/models/model.hpp"
#include "graphics/camera/camera.hpp"

int main(int argc, char **argv) {

    WindowConfig windowConfig;
    windowConfig.title = "Test";
    windowConfig.isMaximised = true;

    Consul consul(windowConfig);

    Camera camera(glm::vec3(0.0f, 0.0f, 2.0f));

    Renderer& renderer = consul.getRenderer();

    IShader* shader = renderer.newShader("shaders/basic.vs", "shaders/basic.fs");

    Model model("assets/shiba/scene.gltf");
    renderer.loadModel(model);

    while (consul.run()) {
        camera.handleInputs(1/60.0f);
        renderer.render(shader, camera);
    }

    return 0;
}