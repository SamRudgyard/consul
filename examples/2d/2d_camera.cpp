#include <memory>
#include <utility>

#include "core/consul.hpp"
#include "core/ecs/components.hpp"
#include "core/project/asset_library.hpp"
#include "core/project/scene.hpp"
#include "graphics/camera/camera_2d.hpp"
#include "graphics/geometry/geometry_2d.hpp"
#include "graphics/material/material.hpp"
#include "graphics/models/model.hpp"

class ExampleScene : public Scene {
public:
    void onInit(std::shared_ptr<AssetLibrary> assets) override {
        camera.setPosition({0.0f, 0.0f});
        defaultShader = assets->importShader("default", "shaders/default_vertex_2d.glsl", "shaders/default_fragment_2d.glsl");

        Mesh mesh = Geometry2D::get()->rect({-0.5f, -0.5f}, {0.5f, 0.5f});

        Material material;
        material.setAlbedo(Colour(20, 200, 200));
        std::shared_ptr<Material> materialAsset = assets->addMaterial("quadMaterial", material);

        std::shared_ptr<Mesh> meshAsset = assets->addMesh("quadMesh", mesh);

        Model model;
        model.addPrimitive(std::move(meshAsset), std::move(materialAsset));
        std::shared_ptr<Model> modelAsset = assets->addModel("quadModel", model);

        const Entity entity = getECS().createEntity();
        getECS().addComponent<Transform>(entity);
        getECS().addComponent<ModelRenderer>(entity, ModelRenderer{std::move(modelAsset), true});
    }

    void onUpdate(std::shared_ptr<AssetLibrary> assets, double deltaTime) override {
        camera.handleInputs(deltaTime);
    }

    Camera* getActiveCamera() override { return &camera; }

private:
    Camera2D camera;
    std::shared_ptr<Shader> defaultShader;
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
