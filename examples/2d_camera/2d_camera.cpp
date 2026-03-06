#include "core/consul.hpp"
#include "core/scene.hpp"
#include "graphics/camera/camera_2d.hpp"
#include "graphics/geometry/geometry_2d.hpp"
#include "graphics/renderable.hpp"

class CubeNode : public Node, public Renderable {
public:
    void initRendering(Renderer& renderer) override {
        Mesh meshData = Geometry2D::get()->rect({-0.5f, -0.5f}, {0.5f, 0.5f});
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
        shader = renderer.newShader("shaders/default_vert.glsl", "shaders/default_frag.glsl");

        CubeNode* cubeNode = getRoot().createChild<CubeNode>();
        cubeNode->setPosition({0.0f, 0.0f, 0.0f});
        cubeNode->initRendering(renderer);
    }

    void onUpdate(float deltaTime) override {
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
    Consul consul;
    consul.loadScene(std::make_unique<ExampleScene>());
    consul.run();
    return 0;
}