#include <memory>
#include <utility>

#include "core/consul.hpp"
#include "core/ecs/components.hpp"
#include "core/engine.hpp"
#include "core/project/scene.hpp"
#include "graphics/camera/camera_2d.hpp"
#include "graphics/colour.hpp"
#include "graphics/geometry/geometry_2d.hpp"
#include "graphics/material/material.hpp"
#include "graphics/models/model.hpp"
#include "graphics/shader/shader.hpp"
#include "utils.hpp"

class RotatingCamera2D : public Camera2D {
public:
    void handleInputs(double deltaTime) override
    {
        InputSystem& input = Engine::get().inputSystem;

        if (input.isKeyDown(KeyboardKey::KEY_A)) {
            rotationDeg += rotationSpeedDeg * deltaTime;
        }
        if (input.isKeyDown(KeyboardKey::KEY_D)) {
            rotationDeg -= rotationSpeedDeg * deltaTime;
        }

        setRotation(rotationDeg);
    }

private:
    float rotationDeg = 0.0f;
    float rotationSpeedDeg = 90.0f;
};

class RotateCameraScene : public Scene {
public:
    void onInit() override
    {
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

        createQuad({-1.25f, 0.0f, 0.0f}, Colour(220, 80, 80));
        createQuad({0.0f, 0.0f, 0.0f}, Colour(240, 200, 90));
        createQuad({1.25f, 0.0f, 0.0f}, Colour(80, 180, 220));
        createQuad({0.0f, 1.15f, 0.0f}, Colour(110, 220, 140));
        createQuad({0.0f, -1.15f, 0.0f}, Colour(170, 120, 230));
    }

    void onUpdate() override
    {
        camera.handleInputs(Engine::get().time.deltaTime);
    }

    Camera* getActiveCamera() override { return &camera; }

private:
    void createQuad(const glm::vec3& position, const Colour& tint)
    {
        Engine& engine = Engine::get();

        Mesh mesh = Geometry2D::get()->rect({-0.35f, -0.35f}, {0.35f, 0.35f});

        Material material;
        material.setAlbedo(tint);
        std::shared_ptr<Material> materialAsset = engine.getMaterialAssetManager()->add(
            "quadMaterial",
            engine.getAssetDefaults().applyToMaterial(material)
        );

        std::shared_ptr<Mesh> meshAsset = engine.getMeshAssetManager()->add("quadMesh", mesh);

        Model model;
        model.addPrimitive(std::move(meshAsset), std::move(materialAsset));
        std::shared_ptr<Model> modelAsset = engine.getModelAssetManager()->add("quadModel", model);

        Transform transform;
        transform.position = position;

        const Entity entity = getECS().createEntity();
        getECS().addComponent<Transform>(entity, transform);
        getECS().addComponent<ModelRenderer>(entity, ModelRenderer{std::move(modelAsset), true});
    }

    RotatingCamera2D camera;
    std::shared_ptr<Shader> defaultShader;
};

int main()
{
    Window window;
    window.title = "2D Example - Custom Rotating Camera";

    Consul consul(window);
    consul.setTargetFPS(60);
    consul.loadScene(std::make_unique<RotateCameraScene>());
    consul.run();
    return 0;
}
