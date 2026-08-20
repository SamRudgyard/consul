#include <cmath>
#include <memory>
#include <utility>

#include "core/consul.hpp"
#include "core/ecs/components.hpp"
#include "core/engine.hpp"
#include "core/window.hpp"
#include "core/project/scene.hpp"
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
        ECS& ecs = getECS();
        VertexShaderAssetManager* vertexShaderManager = engine.getVertexShaderAssetManager();
        FragmentShaderAssetManager* fragmentShaderManager = engine.getFragmentShaderAssetManager();
        ShaderAssetManager* shaderManager = engine.getShaderAssetManager();
        MaterialAssetManager* materialManager = engine.getMaterialAssetManager();
        MeshAssetManager* meshManager = engine.getMeshAssetManager();

        Transform cameraTransform;
        cameraTransform.position = {0.0f, 0.0f, 2.0f};
        CameraComponent camera;
        camera.projectionType = CameraComponent::ProjectionType::PERSPECTIVE;
        camera.fov = 45.0f;
        camera.farPlane = 100.0f;
        cameraEntity = ecs.createEntity();
        ecs.addComponent<Transform>(cameraEntity, cameraTransform);
        ecs.addComponent<CameraComponent>(cameraEntity, camera);

        const std::string vertexShaderSource = readFile("shaders/default_vertex_3d.glsl");
        std::shared_ptr<VertexShader> vertexShader = vertexShaderManager->add("default_VertexShader", VertexShader(vertexShaderSource));
        vertexShaderManager->setSourcePath(vertexShader, "shaders/default_vertex_3d.glsl");

        const std::string fragmentShaderSource = readFile("shaders/default_fragment_3d.glsl");
        std::shared_ptr<FragmentShader> fragmentShader = fragmentShaderManager->add("default_FragmentShader", FragmentShader(fragmentShaderSource));
        fragmentShaderManager->setSourcePath(fragmentShader, "shaders/default_fragment_3d.glsl");

        defaultShader = shaderManager->add("default", Shader(std::move(vertexShader), std::move(fragmentShader)));

        std::shared_ptr<Model> shibaModel = engine.getGLTFImporter().import("shiba", "assets/shiba/scene.gltf");

        const Entity shibaEntity = getECS().createEntity();
        ecs.addComponent<Transform>(shibaEntity);
        ecs.addComponent<ModelRenderer>(shibaEntity, ModelRenderer{std::move(shibaModel), true});

        Mesh mesh = Geometry3D::get()->sphereIcosphere(0.5f, 2);
        Mesh outlineMesh = Geometry3D::get()->sphereIcosphere(0.5f, 2);
        outlineMesh.setDrawMode(DrawMode::LINES);

        Material material;
        material.setAlbedo(Colour(20, 200, 200));
        std::shared_ptr<Material> materialAsset = materialManager->add("cubeMaterial", engine.getAssetDefaults().applyToMaterial(material));

        std::shared_ptr<Mesh> meshAsset = meshManager->add("cubeMesh", mesh);
        std::shared_ptr<Mesh> outlineMeshAsset = meshManager->add("cubeOutlineMesh", outlineMesh);

        Model cube;
        cube.addPrimitive(std::move(meshAsset), std::move(materialAsset));
        cube.addPrimitive(std::move(outlineMeshAsset), engine.getAssetDefaults().getDefaultMaterial());
        std::shared_ptr<Model> cubeModel = engine.getModelAssetManager()->add("cubeModel", cube);

        const Entity cubeEntity = ecs.createEntity();
        ecs.addComponent<Transform>(cubeEntity);
        ecs.addComponent<ModelRenderer>(cubeEntity, ModelRenderer{std::move(cubeModel), true});
    }

    void onUpdate() override
    {
        Engine& engine = Engine::get();
        InputSystem& input = engine.inputSystem;
        Transform& cameraTransform = getECS().getComponent<Transform>(cameraEntity);
        const float deltaTime = static_cast<float>(engine.time.deltaTime);
        const float movementSpeed = input.isKeyDown(KeyboardKey::KEY_LEFT_SHIFT) ? 10.0f : 5.0f;

        const float pitch = cameraTransform.rotation.x;
        const float yaw = cameraTransform.rotation.y;
        const glm::vec3 forward = glm::normalize(glm::vec3(
            -std::sin(yaw) * std::cos(pitch),
            std::sin(pitch),
            -std::cos(yaw) * std::cos(pitch)
        ));
        const glm::vec3 up(0.0f, 1.0f, 0.0f);
        const glm::vec3 right = glm::normalize(glm::cross(forward, up));
        const float movement = movementSpeed * deltaTime;

        if (input.isKeyDown(KeyboardKey::KEY_W)) {
            cameraTransform.position += forward * movement;
        }
        if (input.isKeyDown(KeyboardKey::KEY_A)) {
            cameraTransform.position -= right * movement;
        }
        if (input.isKeyDown(KeyboardKey::KEY_S)) {
            cameraTransform.position -= forward * movement;
        }
        if (input.isKeyDown(KeyboardKey::KEY_D)) {
            cameraTransform.position += right * movement;
        }
        if (input.isKeyDown(KeyboardKey::KEY_SPACE)) {
            cameraTransform.position += up * movement;
        }
        if (input.isKeyDown(KeyboardKey::KEY_LEFT_CONTROL)) {
            cameraTransform.position -= up * movement;
        }

        if (input.isMouseButtonDown(MouseButton::BUTTON_RIGHT)) {
            input.setMouseVisibility(false);

            const glm::vec2 mouseDelta = input.getMousePosition() - input.getPreviousMousePosition();
            constexpr float mouseSensitivity = 0.1f;
            cameraTransform.rotation.x -= glm::radians(mouseDelta.y * mouseSensitivity);
            cameraTransform.rotation.y -= glm::radians(mouseDelta.x * mouseSensitivity);
        } else {
            input.setMouseVisibility(true);
        }
    }

private:
    Entity cameraEntity = 0;
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
