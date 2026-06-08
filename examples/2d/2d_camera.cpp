#include "core/consul.hpp"
#include "core/project/asset_manager.hpp"
#include "core/project/scene.hpp"
#include "graphics/camera/camera_2d.hpp"
#include "graphics/geometry/geometry_2d.hpp"
#include "graphics/material/material.hpp"
#include "graphics/models/model.hpp"
#include "graphics/shader/shader.hpp"

class CubeNode : public Node {
public:
    void initialise(std::shared_ptr<AssetManager> assetManager) {
        Mesh mesh = Geometry2D::get()->rect({-0.5f, -0.5f}, {0.5f, 0.5f});

        std::shared_ptr<Material> material = std::make_shared<Material>();
        material->setAlbedo(Colour(20, 200, 200));
        assetManager->addMaterial("quadMaterial", material);

        mesh.setMaterial(material);

        std::shared_ptr<Model> model = std::make_shared<Model>();
        model->addMesh(mesh);
        modelID = assetManager->addModel("quadModel", model);
    }

protected:
    void onUpdate(std::shared_ptr<AssetManager> assetManager, double deltaTime) override {
        std::shared_ptr<Model> model = assetManager->getModel(modelID);
        if (!model) {
            return;
        }

        model->setTransform(getLocalTransform());
    }

private:
    AssetID modelID;
};

class ExampleScene : public Scene {
public:
    void onInit(std::shared_ptr<AssetManager> assetManager) override {
        camera.setPosition({0.0f, 0.0f});
        assetManager->addShader("default", std::make_shared<Shader>("shaders/default_vertex_2d.glsl", "shaders/default_fragment_2d.glsl"));

        CubeNode* cubeNode = getRoot().createChild<CubeNode>();
        cubeNode->setPosition({0.0f, 0.0f, 0.0f});
        cubeNode->initialise(assetManager);
    }

    void onUpdate(std::shared_ptr<AssetManager> assetManager, double deltaTime) override {
        camera.handleInputs(deltaTime);
    }

    Camera* getActiveCamera() override { return &camera; }

private:
    Camera2D camera;
};

int main()
{
    Window window;
    window.title = "2D Example - Default Camera Movement";
    Consul consul(window);
    ExampleScene scene;
    consul.loadScene(scene);
    consul.run();
    return 0;
}
