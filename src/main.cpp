#include "core/consul.hpp"
#include "core/window.hpp"
#include "graphics/models/model.hpp"
#include "graphics/camera/camera.hpp"
#include "graphics/geometry/geometry.hpp"

#include "glm/gtc/matrix_transform.hpp"

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

    Geometry* geometry = Geometry::get();
    Mesh cube = geometry->cube(0.5f);
    cube.setTint(Colour(80, 120, 220));
    renderer.addMesh(cube, glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.0f, 0.0f)));

    Mesh sphere = geometry->sphereUV(0.5f, 10, 10);
    sphere.setTint(Colour(80, 120, 220));
    renderer.addMesh(sphere, glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 0.0f)));

    while (consul.run()) {
        EngineContext* context = EngineContext::get();
        camera.handleInputs(context->time.deltaTime);
        renderer.render(shader, camera);
    }

    return 0;
}
