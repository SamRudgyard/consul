#include "core/consul.hpp"
#include "core/scene.hpp"
#include "graphics/camera/camera.hpp"
#include "graphics/geometry/geometry_3d.hpp"
#include "graphics/renderable.hpp"

class CubeNode : public Node, public Renderable {
public:
    void initRendering(Renderer& renderer) override {
        Mesh meshData = Geometry3D::get()->cube(1.0f);
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
        camera.setPosition({0.0f, 0.0f, 2.0f});
        camera.setProjectionType(ProjectionType::Orthographic);
        camera.setOrthographic(-2.0f, 2.0f, -2.0f, 2.0f);
        shader = renderer.newShader("shaders/default_vert.glsl", "shaders/default_frag.glsl");

        CubeNode* cubeNode = getRoot().createChild<CubeNode>();
        cubeNode->setPosition({0.0f, 0.0f, 0.0f});
        cubeNode->initRendering(renderer);
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
};

int main()
{
    Consul consul;
    consul.loadScene(std::make_unique<ExampleScene>());
    consul.run();
    return 0;
}