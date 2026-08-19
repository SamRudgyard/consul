#include <memory>
#include <utility>

#include "core/consul.hpp"
#include "core/ecs/components.hpp"
#include "core/engine.hpp"
#include "core/window.hpp"
#include "core/project/scene.hpp"
#include "graphics/camera/camera_3d.hpp"
#include "graphics/geometry/geometry_3d.hpp"
#include "graphics/material/material.hpp"
#include "graphics/models/model.hpp"
#include "graphics/shader/shader.hpp"
#include "utils.hpp"

class ExampleScene : public Scene
{
public:
    ExampleScene() = default;

    void onInit() override
    {
        Engine& engine = Engine::get();

        camera.setProjectionType(ProjectionType::PERSPECTIVE);
        camera.setPosition({0.0f, 0.0f, 2.0f});

        std::shared_ptr<VertexShader> vertexShader = engine.getVertexShaderAssetManager()->add(
            "default_VertexShader",
            VertexShader(readFile("shaders/default_vertex_3d.glsl"))
        );
        engine.getVertexShaderAssetManager()->setSourcePath(vertexShader, "shaders/default_vertex_3d.glsl");

        std::shared_ptr<FragmentShader> fragmentShader = engine.getFragmentShaderAssetManager()->add(
            "default_FragmentShader",
            FragmentShader(readFile("shaders/default_fragment_3d.glsl"))
        );
        engine.getFragmentShaderAssetManager()->setSourcePath(fragmentShader, "shaders/default_fragment_3d.glsl");

        defaultShader = engine.getShaderAssetManager()->add(
            "default",
            Shader(std::move(vertexShader), std::move(fragmentShader))
        );
        std::shared_ptr<Model> shibaModel = engine.getGLTFImporter().import("shiba", "assets/shiba/scene.gltf");

        const Entity shibaEntity = getECS().createEntity();
        getECS().addComponent<Transform>(shibaEntity);
        getECS().addComponent<ModelRenderer>(shibaEntity, ModelRenderer{std::move(shibaModel), true});

        Mesh mesh = Geometry3D::get()->sphereIcosphere(0.5f, 2);
        Mesh outlineMesh = Geometry3D::get()->sphereIcosphere(0.5f, 2);
        outlineMesh.setDrawMode(DrawMode::LINES);

        Material material;
        material.setAlbedo(Colour(20, 200, 200));
        std::shared_ptr<Material> materialAsset = engine.getMaterialAssetManager()->add(
            "cubeMaterial",
            engine.getAssetDefaults().applyToMaterial(material)
        );

        std::shared_ptr<Mesh> meshAsset = engine.getMeshAssetManager()->add("cubeMesh", mesh);
        std::shared_ptr<Mesh> outlineMeshAsset = engine.getMeshAssetManager()->add("cubeOutlineMesh", outlineMesh);

        Model cube;
        cube.addPrimitive(std::move(meshAsset), std::move(materialAsset));
        cube.addPrimitive(std::move(outlineMeshAsset), engine.getAssetDefaults().getDefaultMaterial());
        std::shared_ptr<Model> cubeModel = engine.getModelAssetManager()->add("cubeModel", cube);

        const Entity cubeEntity = getECS().createEntity();
        getECS().addComponent<Transform>(cubeEntity);
        getECS().addComponent<ModelRenderer>(cubeEntity, ModelRenderer{std::move(cubeModel), true});
    }

    void onUpdate() override
    {
        double deltaTime = Engine::get().time.deltaTime;
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
