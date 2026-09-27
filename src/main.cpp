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
        const std::string assetsDirectory = ASSETS_DIR;
        const std::string shadersDirectory = SHADERS_DIR;
        Engine& engine = Engine::get();
        ECS& ecs = getECS();
        VertexShaderAssetManager* vertexShaderManager = engine.getVertexShaderAssetManager();
        FragmentShaderAssetManager* fragmentShaderManager = engine.getFragmentShaderAssetManager();
        ShaderAssetManager* shaderManager = engine.getShaderAssetManager();
        MaterialAssetManager* materialManager = engine.getMaterialAssetManager();
        MeshAssetManager* meshManager = engine.getMeshAssetManager();

        Transform cameraTransform;
        cameraTransform.position = {0.0f, 0.0f, 2.0f};
        Camera camera;
        camera.projectionType = Camera::ProjectionType::PERSPECTIVE;
        camera.fov = 45.0f;
        camera.farPlane = 100.0f;
        cameraEntity = ecs.createEntity();
        ecs.addComponent<Transform>(cameraEntity, cameraTransform);
        ecs.addComponent<Camera>(cameraEntity, camera);

        const std::string vertexShaderSource = readFile(shadersDirectory + "/default_vertex_3d.glsl");
        std::shared_ptr<VertexShader> vertexShader = vertexShaderManager->create("default_VertexShader");
        vertexShaderManager->setSourcePath(vertexShader, shadersDirectory + "/default_vertex_3d.glsl");

        const std::string fragmentShaderSource = readFile(shadersDirectory + "/default_fragment_3d.glsl");
        std::shared_ptr<FragmentShader> fragmentShader = fragmentShaderManager->create("default_FragmentShader");
        fragmentShaderManager->setSourcePath(fragmentShader, shadersDirectory + "/default_fragment_3d.glsl");

        defaultShader = shaderManager->create("defaultShader");

        std::shared_ptr<Model> shibaModel = engine.getGLTFImporter().import("shiba", assetsDirectory + "/shiba/scene.gltf");

        const Entity shibaEntity = getECS().createEntity();
        ecs.addComponent<Transform>(shibaEntity);
        ecs.addComponent<Renderable>(shibaEntity, Renderable{true, shibaModel});

        Mesh mesh = Geometry3D::get()->sphereIcosphere(0.5f, 2);
        Mesh outlineMesh = Geometry3D::get()->sphereIcosphere(0.5f, 2);
        outlineMesh.setDrawMode(DrawMode::LINES);

        Material material;
        material.setAlbedo(Colour(20, 200, 200));
        std::shared_ptr<Material> materialAsset = materialManager->add("icosphereMaterial", material);

        std::shared_ptr<Mesh> meshAsset = meshManager->add("icosphereMesh", mesh);
        std::shared_ptr<Mesh> outlineMeshAsset = meshManager->add("icosphereOutlineMesh", outlineMesh);

        Model icosphere;
        icosphere.addMesh(meshAsset, materialAsset);
        icosphere.addMesh(outlineMeshAsset, std::make_shared<Material>());
        std::shared_ptr<Model> icosphereModel = engine.getModelAssetManager()->add("icosphereModel", icosphere);

        icosphereEntity = ecs.createEntity();
        ecs.addComponent<Transform>(icosphereEntity);
        ecs.addComponent<Renderable>(icosphereEntity, Renderable{true, icosphereModel});
    }

    void onUpdate() override
    {
        Engine& engine = Engine::get();
        InputSystem& input = engine.inputSystem;
        const float deltaTime = static_cast<float>(engine.time.deltaTime);

        Transform& icosphereTransform = getECS().getComponent<Transform>(icosphereEntity);
        const float anglePerSecond = glm::radians(45.0f);
        constexpr float radius = 1.5f;
        icosphereTransform.rotation.y += anglePerSecond * deltaTime;
        icosphereTransform.position = {
            radius * std::cos(icosphereTransform.rotation.y),
            0.0f,
            radius * std::sin(icosphereTransform.rotation.y)
        };

        Transform& cameraTransform = getECS().getComponent<Transform>(cameraEntity);
        const float pitch = cameraTransform.rotation.x;
        const float yaw = cameraTransform.rotation.y;
        const glm::vec3 forward = glm::normalize(glm::vec3(
            -std::sin(yaw) * std::cos(pitch),
            std::sin(pitch),
            -std::cos(yaw) * std::cos(pitch)
        ));
        const glm::vec3 up(0.0f, 1.0f, 0.0f);
        const glm::vec3 right = glm::normalize(glm::cross(forward, up));
        const float movementSpeed = input.isKeyDown(KeyboardKey::KEY_LEFT_SHIFT) ? 10.0f : 5.0f;
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
    Entity icosphereEntity = 0;
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
