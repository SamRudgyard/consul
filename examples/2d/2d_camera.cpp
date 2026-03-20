#include "core/consul.hpp"
#include "core/scene.hpp"
#include "graphics/camera/camera_2d.hpp"
#include "graphics/geometry/geometry_2d.hpp"
#include "graphics/shader.hpp"

class CubeNode : public Node {
public:
    void initialise(Renderer& renderer) {
        mesh = std::make_unique<Mesh>(Geometry2D::get()->rect({-0.5f, -0.5f}, {0.5f, 0.5f}));
        mesh->setTint(Colour(20, 200, 200));
        renderer.uploadMesh(*mesh);
    }

protected:
    void onRender(Renderer& renderer) override {
        if (!mesh) {
            return;
        }

        mesh->setModelMatrix(getWorldTransform());
        renderer.uploadMesh(*mesh);
    }

private:
    std::unique_ptr<Mesh> mesh;
};

class ExampleScene : public Scene {
public:
    ExampleScene()
        : shader("shaders/default_vertex_2d.glsl", "shaders/default_fragment_2d.glsl")
    {}

    void onInit(Renderer& renderer) override {
        camera.setPosition({0.0f, 0.0f});
        renderer.uploadShader(shader);

        CubeNode* cubeNode = getRoot().createChild<CubeNode>();
        cubeNode->setPosition({0.0f, 0.0f, 0.0f});
        cubeNode->initialise(renderer);
    }

    void onUpdate(double deltaTime) override {
        camera.handleInputs(deltaTime);
    }

    void onRender(Renderer& renderer) override {
        renderer.render(shader, camera);
    }

private:
    Shader shader;
    Camera2D camera;
};

int main()
{
    Window window;
    window.title = "2D Example - Default Camera Movement";
    Consul consul(window);
    consul.loadScene(std::make_unique<ExampleScene>());
    consul.run();
    return 0;
}
