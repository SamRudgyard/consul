#include <memory>
#include <utility>

#include "core/consul.hpp"
#include "core/ecs/components.hpp"
#include "core/window.hpp"
#include "core/project/scene.hpp"
#include "graphics/camera/camera_3d.hpp"
#include "graphics/material/material.hpp"
#include "graphics/models/model.hpp"
#include "graphics/geometry/geometry_3d.hpp"

class ExampleScene : public Scene
{
public:
    ExampleScene() = default;

    void onInit(std::shared_ptr<AssetLibrary> assets) override
    {
        camera.setProjectionType(ProjectionType::PERSPECTIVE);
        camera.setPosition({0.0f, 0.0f, 2.0f});
        defaultShader = assets->importShader("default", "shaders/default_vertex_3d.glsl", "shaders/default_fragment_3d.glsl");
        std::shared_ptr<Model> shibaModel = assets->importAsset("shiba", "assets/shiba/scene.gltf");

        const Entity shibaEntity = getECS().createEntity();
        getECS().addComponent<Transform>(shibaEntity);
        getECS().addComponent<ModelRenderer>(shibaEntity, ModelRenderer{std::move(shibaModel), true});

        Mesh mesh = Geometry3D::get()->sphereIcosphere(0.5f, 2);
        Mesh outlineMesh = Geometry3D::get()->sphereIcosphere(0.5f, 2);
        outlineMesh.setDrawMode(DrawMode::LINES);

        Material material;
        material.setAlbedo(Colour(20, 200, 200));
        std::shared_ptr<Material> materialAsset = assets->addMaterial("cubeMaterial", material);

        std::shared_ptr<Mesh> meshAsset = assets->addMesh("cubeMesh", mesh);
        std::shared_ptr<Mesh> outlineMeshAsset = assets->addMesh("cubeOutlineMesh", outlineMesh);

        Model cube;
        cube.addPrimitive(std::move(meshAsset), std::move(materialAsset));
        cube.addPrimitive(std::move(outlineMeshAsset), assets->getDefaultMaterial());
        std::shared_ptr<Model> cubeModel = assets->addModel("cubeModel", cube);

        const Entity cubeEntity = getECS().createEntity();
        getECS().addComponent<Transform>(cubeEntity);
        getECS().addComponent<ModelRenderer>(cubeEntity, ModelRenderer{std::move(cubeModel), true});
    }

    void onUpdate(std::shared_ptr<AssetLibrary> assets, double deltaTime) override
    {
        camera.handleInputs(deltaTime);
    }

    // A bit hacky, but will do for the moment
    Camera* getActiveCamera() override { return &camera; }

private:
    Camera3D camera;
    std::shared_ptr<Shader> defaultShader;
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
