#include "core/consul.hpp"
#include "core/window.hpp"
#include "core/scene.hpp"
#include "graphics/models/model.hpp"
#include "graphics/camera/camera.hpp"
#include "graphics/geometry/geometry_3d.hpp"

#include "glm/gtc/matrix_transform.hpp"

class ExampleScene : public Scene
{
public:
    ExampleScene()
        : camera(glm::vec3(0.0f, 0.0f, 2.0f)), model("assets/shiba/scene.gltf") {}

    void initialise(Renderer& renderer) override
    {
        shader = renderer.newShader("shaders/default_vert.glsl", "shaders/default_frag.glsl");
        renderer.loadModel(model);

        Geometry3D* geometry3d = Geometry3D::get();
        Mesh cube = geometry3d->cube(0.5f);
        cube.setTint(Colour(126, 0, 126));
        renderer.addMesh(cube, glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.0f, 0.0f)));

        Mesh cylinder = geometry3d->cylinder(0.5f, 0.2f, 2.0f, 20);
        cylinder.setTint(Colour(235, 110, 52));
        renderer.addMesh(cylinder, glm::translate(glm::mat4(1.0f), glm::vec3(0.f, 1.0f, 0.0f)));

        Mesh cone = geometry3d->cone(1.0f, 2.0f, 20);
        cone.setTint(Colour(66, 245, 132));
        renderer.addMesh(cone, glm::translate(glm::mat4(1.0f), glm::vec3(0.f, -2.0f, 0.0f)));

        Mesh sphere = geometry3d->sphereIcosphere(0.5f, 1);
        sphere.setTint(Colour(80, 120, 220));
        renderer.addMesh(sphere, glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 0.0f)));

        Mesh pyramid = geometry3d->pyramidSquare(1.0f, 1.0f);
        pyramid.setTint(Colour(191, 66, 245));
        renderer.addMesh(pyramid, glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, 0.0f, 0.0f)));

        Mesh capsule = geometry3d->capsule(0.5f, 2.0f, 10, 10);
        capsule.setDrawMode(DrawMode::LINES);
        renderer.addMesh(capsule, glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, 0.0f, 0.0f)));

        Mesh plane = geometry3d->plane(10.f, 10.f);
        renderer.addMesh(plane, glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        
        Mesh line = geometry3d->line({0.f, 0.f, 0.f}, {2.0f, 5.0f, 0.f});
        renderer.addMesh(line);

        isInitialised = true;
    }

    void update(float deltaTime) override
    {
        camera.handleInputs(deltaTime);
    }

    void render(Renderer& renderer) override
    {
        renderer.render(shader, camera);
    }

    void shutdown() override
    {
        delete shader;
        shader = nullptr;
    }

private:
    Camera camera;
    IShader* shader = nullptr;
    Model model;
};

int main(int argc, char **argv)
{
    Window window;
    window.title = "Test";
    window.isMaximised = true;

    Consul consul(window);

    ExampleScene scene;
    consul.loadScene(&scene);
    consul.run();

    return 0;
}
