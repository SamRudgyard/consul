#include <memory>
#include <utility>

#include "core/consul.hpp"
#include "core/window.hpp"
#include "core/project/scene.hpp"
#include "graphics/camera/camera_3d.hpp"
#include "graphics/material/material.hpp"
#include "graphics/models/model.hpp"
#include "graphics/geometry/geometry_3d.hpp"

class CubeNode : public Node
{
public:
    void initialise(std::shared_ptr<AssetLibrary> assets)
    {
        Mesh mesh = Geometry3D::get()->sphereIcosphere(0.5f, 2);
        Mesh outlineMesh = Geometry3D::get()->sphereIcosphere(0.5f, 2);
        outlineMesh.setDrawMode(DrawMode::LINES);

        Material material;
        material.setAlbedo(Colour(20, 200, 200));
        std::shared_ptr<Material> materialAsset = assets->addMaterial("cubeMaterial", material);

        mesh.setMaterial(std::move(materialAsset));
        std::shared_ptr<Mesh> meshAsset = assets->addMesh("cubeMesh", mesh);
        std::shared_ptr<Mesh> outlineMeshAsset = assets->addMesh("cubeOutlineMesh", outlineMesh);

        Model model;
        model.addMesh(std::move(meshAsset));
        model.addMesh(std::move(outlineMeshAsset));
        modelID = assets->addModel("cubeModel", model);
    }

protected:
    void onUpdate(std::shared_ptr<AssetLibrary> assets, double deltaTime) override
    {
        static float r = 1.5f;
        static float anglePerSecond = glm::radians(45.0f);

        incrementRotationRad({0.0f, anglePerSecond*((float)deltaTime), 0.0f});
        float currentAngle = getRotationY();
        setPosition({r*std::cos(currentAngle), 0.0f, r*std::sin(currentAngle)});

        // Update the model's mesh data to reflect the new position/rotation
        std::shared_ptr<Model> model = assets->getModel(modelID);
        model->setTransform(getWorldTransform());
    }
private:
    AssetID modelID;
};

class ExampleScene : public Scene
{
public:
    ExampleScene() = default;

    void onInit(std::shared_ptr<AssetLibrary> assets) override
    {
        camera.setProjectionType(ProjectionType::PERSPECTIVE);
        camera.setPosition({0.0f, 0.0f, 2.0f});
        assets->importShader("default", "shaders/default_vertex_3d.glsl", "shaders/default_fragment_3d.glsl");
        assets->importAsset("shiba", "assets/shiba/scene.gltf");

        CubeNode* rotatingCube = getRoot().createChild<CubeNode>();
        rotatingCube->initialise(assets);
    }

    void onUpdate(std::shared_ptr<AssetLibrary> assets, double deltaTime) override
    {
        camera.handleInputs(deltaTime);
    }

    // A bit hacky, but will do for the moment
    Camera* getActiveCamera() override { return &camera; }

private:
    Camera3D camera;
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
