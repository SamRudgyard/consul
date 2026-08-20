#include <algorithm>
#include <memory>
#include <utility>

#include "core/consul.hpp"
#include "core/ecs/components.hpp"
#include "core/engine.hpp"
#include "core/project/scene.hpp"
#include "graphics/geometry/geometry_2d.hpp"
#include "graphics/material/material.hpp"
#include "graphics/models/model.hpp"
#include "graphics/shader/shader.hpp"
#include "utils.hpp"

class ExampleScene : public Scene {
public:
    void onInit() override {
        Engine& engine = Engine::get();
        ECS& ecs = getECS();
        VertexShaderAssetManager* vertexShaderManager = engine.getVertexShaderAssetManager();
        FragmentShaderAssetManager* fragmentShaderManager = engine.getFragmentShaderAssetManager();
        ShaderAssetManager* shaderManager = engine.getShaderAssetManager();
        MaterialAssetManager* materialManager = engine.getMaterialAssetManager();
        MeshAssetManager* meshManager = engine.getMeshAssetManager();

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

        Mesh mesh = Geometry2D::get()->rect({-0.5f, -0.5f}, {0.5f, 0.5f});

        Material material;
        material.setAlbedo(Colour(20, 200, 200));
        std::shared_ptr<Material> materialAsset = materialManager->add("quadMaterial", engine.getAssetDefaults().applyToMaterial(material));

        std::shared_ptr<Mesh> meshAsset = meshManager->add("quadMesh", mesh);

        Model model;
        model.addPrimitive(std::move(meshAsset), std::move(materialAsset));
        std::shared_ptr<Model> modelAsset = engine.getModelAssetManager()->add("quadModel", model);

        const Entity entity = ecs.createEntity();
        ecs.addComponent<Transform>(entity);
        ecs.addComponent<ModelRenderer>(entity, ModelRenderer{std::move(modelAsset), true});
    }

    void onUpdate() override {
        Engine& engine = Engine::get();
        InputSystem& input = engine.inputSystem;
        ECS& ecs = getECS();
        Transform& cameraTransform = ecs.getComponent<Transform>(cameraEntity);
        Camera& camera = ecs.getComponent<Camera>(cameraEntity);
        const float deltaTime = static_cast<float>(engine.time.deltaTime);
        constexpr float movementSpeed = 5.0f;

        if (input.isKeyDown(KeyboardKey::KEY_W)) {
            cameraTransform.position.y += movementSpeed * deltaTime;
        }
        if (input.isKeyDown(KeyboardKey::KEY_A)) {
            cameraTransform.position.x -= movementSpeed * deltaTime;
        }
        if (input.isKeyDown(KeyboardKey::KEY_S)) {
            cameraTransform.position.y -= movementSpeed * deltaTime;
        }
        if (input.isKeyDown(KeyboardKey::KEY_D)) {
            cameraTransform.position.x += movementSpeed * deltaTime;
        }

        const float scrollOffset = input.getMouseScrollOffset().y;
        if (scrollOffset != 0.0f) {
            constexpr float defaultViewHeight = 2.0f;
            float zoom = defaultViewHeight / camera.orthographicHeight;
            zoom = std::clamp(zoom + 0.1f * scrollOffset, 0.1f, 1.0f);
            camera.orthographicHeight = defaultViewHeight / zoom;
        }
    }

private:
    Entity cameraEntity = 0;
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
