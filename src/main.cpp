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
    void initialise(std::shared_ptr<AssetManager> assetManager)
    {
        Mesh mesh = Geometry3D::get()->sphereIcosphere(0.5f, 2);
        Mesh outlineMesh = Geometry3D::get()->sphereIcosphere(0.5f, 2);
        outlineMesh.setDrawMode(DrawMode::LINES);

        std::shared_ptr<Material> material = std::make_shared<Material>();
        material->setAlbedo(Colour(20, 200, 200));
        assetManager->addMaterial("cubeMaterial", material);

        mesh.setMaterial(material);

        std::shared_ptr<Model> model = std::make_shared<Model>();
        model->addMesh(mesh);
        model->addMesh(outlineMesh);
        modelID = assetManager->addModel("cubeModel", model);
    }

protected:
    void onUpdate(std::shared_ptr<AssetManager> assetManager, double deltaTime) override
    {
        static float r = 1.5f;
        static float anglePerSecond = glm::radians(45.0f);

        incrementRotationRad({0.0f, anglePerSecond*((float)deltaTime), 0.0f});
        float currentAngle = getRotationY();
        setPosition({r*std::cos(currentAngle), 0.0f, r*std::sin(currentAngle)});

        // Update the model's mesh data to reflect the new position/rotation
        std::shared_ptr<Model> model = assetManager->getModel(modelID);
        model->setTransform(getWorldTransform());
    }
private:
    AssetID modelID;
};

class ExampleScene : public Scene
{
public:
    ExampleScene() = default;

    void onInit(std::shared_ptr<AssetManager> assetManager) override
    {
        camera.setProjectionType(ProjectionType::PERSPECTIVE);
        camera.setPosition({0.0f, 0.0f, 2.0f});
        assetManager->addShader("default", std::make_shared<Shader>("shaders/default_vertex_3d.glsl", "shaders/default_fragment_3d.glsl"));
        assetManager->addModel("shiba", std::make_shared<Model>("assets/shiba/scene.gltf"));

        CubeNode* rotatingCube = getRoot().createChild<CubeNode>();
        rotatingCube->initialise(assetManager);
    }

    void onUpdate(std::shared_ptr<AssetManager> assetManager, double deltaTime) override
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
