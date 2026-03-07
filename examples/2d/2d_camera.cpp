#include "core/consul.hpp"
#include "core/scene.hpp"
#include "graphics/camera/camera_2d.hpp"
#include "graphics/geometry/geometry_2d.hpp"
#include "graphics/renderable.hpp"

class CubeNode : public Node, public Renderable {
public:
    void initRendering(Renderer& renderer) override {
        Mesh meshData = Geometry2D::get()->rect({-0.5f, -0.5f}, {0.5f, 0.5f});
        meshData.setTint(Colour(20, 200, 200));
        mesh = renderer.addMesh(meshData);
    }

    void syncToRenderer() override {
        if (mesh) {
            mesh->setModelMatrix(getWorldTransform());
        }
    }

protected:
    void onRender(Renderer& renderer) override {
        syncToRenderer();
    }

private:
    RenderableMesh* mesh = nullptr;
};

class ExampleScene : public Scene {
public:
    ExampleScene() {}

    void onInit(Renderer& renderer) override {
        camera.setPosition({0.0f, 0.0f});
        shader = renderer.newShader("shaders/default_vertex_2d.glsl", "shaders/default_fragment_2d.glsl");

        CubeNode* cubeNode = getRoot().createChild<CubeNode>();
        cubeNode->setPosition({0.0f, 0.0f, 0.0f});
        cubeNode->initRendering(renderer);
    }

    void onUpdate(double deltaTime) override {
        camera.handleInputs(deltaTime);
    }

    void onRender(Renderer& renderer) override {
        renderer.render(shader, camera);
    }

    void onShutdown() override {
        delete shader;
        shader = nullptr;
    }

private:
    IShader* shader = nullptr;
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
