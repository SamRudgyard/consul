#include <memory>
#include <utility>

#include "core/consul.hpp"
#include "core/ecs/components.hpp"
#include "core/engine.hpp"
#include "core/project/scene.hpp"
#include "graphics/colour.hpp"
#include "graphics/geometry/geometry_2d.hpp"
#include "graphics/material/material.hpp"
#include "graphics/models/model.hpp"
#include "graphics/shader/shader.hpp"
#include "utils.hpp"

class RotateCameraScene : public Scene {
public:
    void onInit() override
    {
        Engine& engine = Engine::get();
        ECS& ecs = getECS();
        VertexShaderAssetManager* vertexShaderManager = engine.getVertexShaderAssetManager();
        FragmentShaderAssetManager* fragmentShaderManager = engine.getFragmentShaderAssetManager();
        ShaderAssetManager* shaderManager = engine.getShaderAssetManager();

        Camera camera;
        camera.projectionType = Camera::ProjectionType::ORTHOGRAPHIC;
        camera.nearPlane = -1.0f;
        camera.farPlane = 1.0f;
        cameraEntity = ecs.createEntity();
        ecs.addComponent<Transform>(cameraEntity);
        ecs.addComponent<Camera>(cameraEntity, camera);

        const std::string vertexShaderSource = readFile("shaders/default_vertex_2d.glsl");
        std::shared_ptr<VertexShader> vertexShader = vertexShaderManager->add("default_VertexShader", VertexShader(vertexShaderSource));
        vertexShaderManager->setSourcePath(vertexShader, "shaders/default_vertex_2d.glsl");

        const std::string fragmentShaderSource = readFile("shaders/default_fragment_2d.glsl");
        std::shared_ptr<FragmentShader> fragmentShader = fragmentShaderManager->add("default_FragmentShader", FragmentShader(fragmentShaderSource));
        fragmentShaderManager->setSourcePath(fragmentShader, "shaders/default_fragment_2d.glsl");

        defaultShader = shaderManager->add("default", Shader(std::move(vertexShader), std::move(fragmentShader)));

        createQuad({-1.25f, 0.0f, 0.0f}, Colour(220, 80, 80));
        createQuad({0.0f, 0.0f, 0.0f}, Colour(240, 200, 90));
        createQuad({1.25f, 0.0f, 0.0f}, Colour(80, 180, 220));
        createQuad({0.0f, 1.15f, 0.0f}, Colour(110, 220, 140));
        createQuad({0.0f, -1.15f, 0.0f}, Colour(170, 120, 230));
    }

    void onUpdate() override
    {
        Engine& engine = Engine::get();
        InputSystem& input = engine.inputSystem;
        Transform& cameraTransform = getECS().getComponent<Transform>(cameraEntity);
        const float rotation = glm::radians(90.0f) * static_cast<float>(engine.time.deltaTime);

        if (input.isKeyDown(KeyboardKey::KEY_A)) {
            cameraTransform.rotation.z += rotation;
        }
        if (input.isKeyDown(KeyboardKey::KEY_D)) {
            cameraTransform.rotation.z -= rotation;
        }
    }

private:
    void createQuad(const glm::vec3& position, const Colour& tint)
    {
        Engine& engine = Engine::get();
        ECS& ecs = getECS();
        MaterialAssetManager* materialManager = engine.getMaterialAssetManager();
        MeshAssetManager* meshManager = engine.getMeshAssetManager();

        Mesh mesh = Geometry2D::get()->rect({-0.35f, -0.35f}, {0.35f, 0.35f});

        Material material;
        material.setAlbedo(tint);
        std::shared_ptr<Material> materialAsset = materialManager->add("quadMaterial", engine.getAssetDefaults().applyToMaterial(material));

        std::shared_ptr<Mesh> meshAsset = meshManager->add("quadMesh", mesh);

        Model model;
        model.addPrimitive(std::move(meshAsset), std::move(materialAsset));
        std::shared_ptr<Model> modelAsset = engine.getModelAssetManager()->add("quadModel", model);

        Transform transform;
        transform.position = position;

        const Entity entity = ecs.createEntity();
        ecs.addComponent<Transform>(entity, transform);
        ecs.addComponent<ModelRenderer>(entity, ModelRenderer{std::move(modelAsset), true});
    }

    Entity cameraEntity = 0;
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
