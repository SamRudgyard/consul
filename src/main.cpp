#include <memory>

#include "core/consul.hpp"
#include "core/window.hpp"
#include "core/project/scene.hpp"
#include "graphics/camera/camera_3d.hpp"
#include "graphics/shader/shader.hpp"
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
        AssetID materialID = assets->addMaterial("cubeMaterial", material);

        mesh.setMaterial(materialID);
        AssetID meshID = assets->addMesh("cubeMesh", mesh);
        AssetID outlineMeshID = assets->addMesh("cubeOutlineMesh", outlineMesh);

        Model model;
        model.addMesh(meshID);
        model.addMesh(outlineMeshID);
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
        assets->addShader("default", Shader("shaders/default_vertex_3d.glsl", "shaders/default_fragment_3d.glsl"));
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
    ExampleScene scene = ExampleScene();
    consul.loadScene(scene);
    consul.run();

    return 0;
}
