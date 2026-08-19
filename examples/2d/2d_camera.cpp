#include <memory>
#include <utility>

#include "core/consul.hpp"
#include "core/ecs/components.hpp"
#include "core/engine.hpp"
#include "core/project/scene.hpp"
#include "graphics/camera/camera_2d.hpp"
#include "graphics/geometry/geometry_2d.hpp"
#include "graphics/material/material.hpp"
#include "graphics/models/model.hpp"
#include "graphics/shader/shader.hpp"
#include "utils.hpp"

class ExampleScene : public Scene {
public:
    void onInit() override {
        Engine& engine = Engine::get();

        camera.setPosition({0.0f, 0.0f});

        std::shared_ptr<VertexShader> vertexShader = engine.getVertexShaderAssetManager()->add(
            "default_VertexShader",
            VertexShader(readFile("shaders/default_vertex_2d.glsl"))
        );
        engine.getVertexShaderAssetManager()->setSourcePath(vertexShader, "shaders/default_vertex_2d.glsl");

        std::shared_ptr<FragmentShader> fragmentShader = engine.getFragmentShaderAssetManager()->add(
            "default_FragmentShader",
            FragmentShader(readFile("shaders/default_fragment_2d.glsl"))
        );
        engine.getFragmentShaderAssetManager()->setSourcePath(fragmentShader, "shaders/default_fragment_2d.glsl");

        defaultShader = engine.getShaderAssetManager()->add(
            "default",
            Shader(std::move(vertexShader), std::move(fragmentShader))
        );

        Mesh mesh = Geometry2D::get()->rect({-0.5f, -0.5f}, {0.5f, 0.5f});

        Material material;
        material.setAlbedo(Colour(20, 200, 200));
        std::shared_ptr<Material> materialAsset = engine.getMaterialAssetManager()->add(
            "quadMaterial",
            engine.getAssetDefaults().applyToMaterial(material)
        );

        std::shared_ptr<Mesh> meshAsset = engine.getMeshAssetManager()->add("quadMesh", mesh);

        Model model;
        model.addPrimitive(std::move(meshAsset), std::move(materialAsset));
        std::shared_ptr<Model> modelAsset = engine.getModelAssetManager()->add("quadModel", model);

        const Entity entity = getECS().createEntity();
        getECS().addComponent<Transform>(entity);
        getECS().addComponent<ModelRenderer>(entity, ModelRenderer{std::move(modelAsset), true});
    }

    void onUpdate() override {
        camera.handleInputs(Engine::get().time.deltaTime);
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
