#include <memory>

#include "core/consul.hpp"
#include "core/window.hpp"
#include "core/scene.hpp"
#include "graphics/renderable.hpp"
#include "graphics/models/model.hpp"
#include "graphics/geometry/geometry_3d.hpp"
#include "graphics/mesh/renderable_mesh.hpp"

class CubeNode : public Node, public Renderable
{
public:
    void initRendering(Renderer& renderer) override
    {
        Mesh cube = Geometry3D::get()->cube(0.4f);
        cube.setTint(Colour(20, 200, 200));
        mesh = renderer.addMesh(cube);
    }

    void syncToRenderer() override
    {
        if (mesh) {
            mesh->setModelMatrix(getWorldTransform());
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
    float angle = 0.0f;
};

class ExampleScene : public Scene
{
public:
    ExampleScene() : model("assets/shiba/scene.gltf") {};

    void onInit(Renderer& renderer) override
    {
        camera.setPosition({0.0f, 0.0f, 2.0f});
        shader = renderer.newShader("shaders/default_vert.glsl", "shaders/default_frag.glsl");
        renderer.loadModel(model);

        std::unique_ptr<CubeNode> rotatingCube = std::make_unique<CubeNode>();
        rotatingCube->initRendering(renderer);
        getRoot().addChild(std::move(rotatingCube));
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
