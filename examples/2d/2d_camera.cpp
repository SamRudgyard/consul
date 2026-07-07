#include "core/consul.hpp"
#include "core/project/asset_library.hpp"
#include "core/project/scene.hpp"
#include "graphics/camera/camera_2d.hpp"
#include "graphics/geometry/geometry_2d.hpp"
#include "graphics/material/material.hpp"
#include "graphics/models/model.hpp"
#include "graphics/shader/shader.hpp"

class CubeNode : public Node {
public:
    void initialise(std::shared_ptr<AssetLibrary> assets) {
        Mesh mesh = Geometry2D::get()->rect({-0.5f, -0.5f}, {0.5f, 0.5f});

        Material material;
        material.setAlbedo(Colour(20, 200, 200));
        AssetID materialID = assets->addMaterial("quadMaterial", material);

        mesh.setMaterial(materialID);
        AssetID meshID = assets->addMesh("quadMesh", mesh);

        Model model;
        model.addMesh(meshID);
        modelID = assets->addModel("quadModel", model);
    }

protected:
    void onUpdate(std::shared_ptr<AssetLibrary> assets, double deltaTime) override {
        std::shared_ptr<Model> model = assets->getModel(modelID);
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
    void onInit(std::shared_ptr<AssetLibrary> assets) override {
        camera.setPosition({0.0f, 0.0f});
        assets->addShader("default", Shader("shaders/default_vertex_2d.glsl", "shaders/default_fragment_2d.glsl"));

        CubeNode* cubeNode = getRoot().createChild<CubeNode>();
        cubeNode->setPosition({0.0f, 0.0f, 0.0f});
        cubeNode->initialise(assets);
    }

    void onUpdate(std::shared_ptr<AssetLibrary> assets, double deltaTime) override {
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
