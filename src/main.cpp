#include "core/consul.hpp"
#include "core/window.hpp"
#include "core/scene.hpp"
#include "graphics/models/model.hpp"
#include "graphics/camera/camera.hpp"
#include "graphics/geometry/geometry_3d.hpp"
#include "graphics/mesh/renderable_mesh.hpp"

#include "glm/gtc/matrix_transform.hpp"
#include <memory>

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
        Mesh rotatingCube = geometry3d->cube(0.4f);
        rotatingCube.setTint(Colour(20, 200, 200));
        rotatingMesh = renderer.addMesh(rotatingCube);

        Node* rotatingNode = getRoot().createChild();
        rotatingNode->setUpdateCallback([this](Node& node, float dt) {
            rotationAngle += dt;
            node.setRotationRad({0.0f, rotationAngle, 0.0f});
            node.setPosition({0.0f, 1.5f, 0.0f});
        });
        rotatingNode->setRenderCallback([this](Node& node, Renderer& renderer) {
            if (rotatingMesh) {
                rotatingMesh->setModelMatrix(node.getWorldTransform());
            }
        });
    }

    void update(float deltaTime) override
    {
        camera.handleInputs(deltaTime);
        updateSceneGraph(deltaTime);
    }

    void render(Renderer& renderer) override
    {
        renderSceneGraph(renderer);
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
    RenderableMesh* rotatingMesh = nullptr;
    float rotationAngle = 0.0f;
};

int main(int argc, char **argv)
{
    Window window;
    window.title = "Test";
    window.isMaximised = true;

    Consul consul(window);
    consul.loadScene(std::make_unique<ExampleScene>());
    consul.run();

    return 0;
}
