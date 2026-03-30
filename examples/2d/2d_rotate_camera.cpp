#include "core/consul.hpp"
#include "core/scene.hpp"
#include "graphics/camera/camera_2d.hpp"
#include "graphics/colour.hpp"
#include "graphics/geometry/geometry_2d.hpp"
#include "graphics/shader/shader.hpp"

class RotatingCamera2D : public Camera2D {
public:
    void handleInputs(double deltaTime) override
    {
        InputSystem& input = EngineContext::get()->inputSystem;

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

class ColouredQuadNode : public Node {
public:
    explicit ColouredQuadNode(const Colour& tint)
        : tint(tint)
    {}

    void initialise(Renderer& renderer)
    {
        mesh = std::make_unique<Mesh>(Geometry2D::get()->rect({-0.35f, -0.35f}, {0.35f, 0.35f}));
        mesh->setTint(tint);
        renderer.uploadMesh(*mesh);
    }

protected:
    void onRender(Renderer& renderer) override
    {
        if (!mesh) {
            return;
        }

        mesh->setModelMatrix(getWorldTransform());
        renderer.uploadMesh(*mesh);
    }

private:
    Colour tint;
    std::unique_ptr<Mesh> mesh;
};

class RotateCameraScene : public Scene {
public:
    RotateCameraScene()
        : shader("shaders/default_vertex_2d.glsl", "shaders/default_fragment_2d.glsl")
    {}

    void onInit(Renderer& renderer) override
    {
        camera.setPosition({0.0f, 0.0f});
        renderer.uploadShader(shader);

        createQuad(renderer, {-1.25f, 0.0f, 0.0f}, Colour(220, 80, 80));
        createQuad(renderer, {0.0f, 0.0f, 0.0f}, Colour(240, 200, 90));
        createQuad(renderer, {1.25f, 0.0f, 0.0f}, Colour(80, 180, 220));
        createQuad(renderer, {0.0f, 1.15f, 0.0f}, Colour(110, 220, 140));
        createQuad(renderer, {0.0f, -1.15f, 0.0f}, Colour(170, 120, 230));
    }

    void onUpdate(double deltaTime) override
    {
        camera.handleInputs(deltaTime);
    }

    void onRender(Renderer& renderer) override
    {
        renderer.render(shader, camera);
    }

private:
    void createQuad(Renderer& renderer, const glm::vec3& position, const Colour& tint)
    {
        ColouredQuadNode* quad = getRoot().createChild<ColouredQuadNode>(tint);
        quad->setPosition(position);
        quad->initialise(renderer);
    }

    Shader shader;
    RotatingCamera2D camera;
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
