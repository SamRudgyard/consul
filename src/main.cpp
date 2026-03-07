#include <memory>

#include "core/consul.hpp"
#include "core/window.hpp"
#include "core/scene.hpp"
#include "graphics/camera/camera_3d.hpp"
#include "graphics/renderable.hpp"
#include "graphics/models/model.hpp"
#include "graphics/geometry/geometry_3d.hpp"
#include "graphics/mesh/renderable_mesh.hpp"

class CubeNode : public Node, public Renderable
{
public:
    void initRendering(Renderer& renderer) override
    {
        Mesh icosphere = Geometry3D::get()->sphereIcosphere(0.5f, 2);
        Mesh icosphereOutline = Geometry3D::get()->sphereIcosphere(0.5f, 2);
        icosphereOutline.setDrawMode(DrawMode::LINES);
        icosphere.setTint(Colour(20, 200, 200));
        mesh = renderer.addMesh(icosphere);
        outlineMesh = renderer.addMesh(icosphereOutline);
    }

    void syncToRenderer() override
    {
        if (mesh) {
            mesh->setModelMatrix(getWorldTransform());
        }
        if (outlineMesh) {
            outlineMesh->setModelMatrix(getWorldTransform());
        }
    }

protected:
    void onUpdate(float dt) override
    {
        angle += dt;
        setRotationRad({0.0f, angle, 0.0f});
        static float r = 1.5f;
        setPosition({r * std::cos(angle), 0.0f, r * std::sin(angle)});
    }

    void onRender(Renderer&) override
    {
        syncToRenderer();
    }

private:
    RenderableMesh* mesh = nullptr;
    RenderableMesh* outlineMesh = nullptr;
    float angle = 0.0f;
};

class ExampleScene : public Scene
{
public:
    ExampleScene() : model("assets/shiba/scene.gltf") {};

    void onInit(Renderer& renderer) override
    {
        camera.setProjectionType(ProjectionType::PERSPECTIVE);
        camera.setPosition({0.0f, 0.0f, 2.0f});
        shader = renderer.newShader("shaders/default_vertex_3d.glsl", "shaders/default_fragment_3d.glsl");
        renderer.loadModel(model);

        CubeNode* rotatingCube = getRoot().createChild<CubeNode>();
        rotatingCube->initRendering(renderer);
    }

    void onUpdate(float deltaTime) override
    {
        camera.handleInputs(deltaTime);
    }

    void onRender(Renderer& renderer) override
    {
        renderer.render(shader, camera);
    }

    void onShutdown() override
    {
        delete shader;
        shader = nullptr;
    }

private:
    Camera3D camera;
    IShader* shader = nullptr;
    Model model;
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
