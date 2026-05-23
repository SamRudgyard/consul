#include <memory>

#include "core/consul.hpp"
#include "core/window.hpp"
#include "core/scene.hpp"
#include "graphics/camera/camera_3d.hpp"
#include "graphics/shader/shader.hpp"
#include "graphics/material/material.hpp"
#include "graphics/models/model.hpp"
#include "graphics/geometry/geometry_3d.hpp"

class CubeNode : public Node
{
public:
    void initialise(Renderer& renderer)
    {
        Mesh mesh = Geometry3D::get()->sphereIcosphere(0.5f, 2);
        Mesh outlineMesh = Geometry3D::get()->sphereIcosphere(0.5f, 2);
        outlineMesh.setDrawMode(DrawMode::LINES);

        material = std::make_shared<Material>();
        material->setUniform("albedo", Colour(20, 200, 200));

        mesh.setMaterial(material);

        model.addMesh(mesh);
        model.addMesh(outlineMesh);

        renderer.uploadModel(model);
    }

protected:
    void onUpdate(double deltaTime) override
    {
        angle += deltaTime;
        setRotationRad({0.0f, angle, 0.0f});
        static float r = 1.5f;
        setPosition({r * std::cos(angle), 0.0f, r * std::sin(angle)});
    }

    void onRender(Renderer& renderer) override
    {
        model.setTransform(getWorldTransform());
        renderer.uploadModel(model);
    }

private:
    Model model;
    std::shared_ptr<Material> material;
    float angle = 0.0f;
};

class ExampleScene : public Scene
{
public:
    ExampleScene()
        : model("assets/shiba/scene.gltf"),
          shader("shaders/default_vertex_3d.glsl", "shaders/default_fragment_3d.glsl")
    {}

    void onInit(Renderer& renderer) override
    {
        camera.setProjectionType(ProjectionType::PERSPECTIVE);
        camera.setPosition({0.0f, 0.0f, 2.0f});
        renderer.uploadShader(shader);
        renderer.uploadModel(model);

        CubeNode* rotatingCube = getRoot().createChild<CubeNode>();
        rotatingCube->initialise(renderer);
    }

    void onUpdate(double deltaTime) override
    {
        camera.handleInputs(deltaTime);
    }

    void onRender(Renderer& renderer) override
    {
        renderer.render(shader, camera);
    }

private:
    Camera3D camera;
    Model model;
    Shader shader;
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
