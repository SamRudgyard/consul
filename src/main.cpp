#include "core/consul.hpp"
#include "core/window.hpp"
#include "graphics/models/model.hpp"
#include "graphics/camera/camera.hpp"

int main(int argc, char **argv) {

    Window window;
    window.title = "Test";
    window.isMaximised = true;

    Consul consul(window);

    Camera camera(glm::vec3(0.0f, 0.0f, 2.0f));

    Renderer& renderer = consul.getRenderer();

    IShader* shader = renderer.newShader("shaders/basic.vs", "shaders/basic.fs");

    Model model("assets/shiba/scene.gltf");
    renderer.loadModel(model);

    while (consul.run()) {
        EngineContext* context = EngineContext::get();
        camera.handleInputs(context->time.deltaTime);
        renderer.render(shader, camera);
    }

    return 0;
}