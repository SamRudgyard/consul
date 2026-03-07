#include "core/consul.hpp"
#include "core/scene.hpp"
#include "graphics/camera/camera_2d.hpp"
#include "graphics/colour.hpp"
#include "graphics/geometry/geometry_2d.hpp"
#include "graphics/renderable.hpp"

class RotatingCamera2D : public Camera2D {
public:
    void handleInputs(float deltaTime) override
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

class ColouredQuadNode : public Node, public Renderable {
public:
    explicit ColouredQuadNode(const Colour& tint)
        : tint(tint)
    {}

    void initRendering(Renderer& renderer) override
    {
        Mesh meshData = Geometry2D::get()->rect({-0.35f, -0.35f}, {0.35f, 0.35f});
        meshData.setTint(tint);
        mesh = renderer.addMesh(meshData);
    }

    void syncToRenderer() override
    {
        if (mesh) {
            mesh->setModelMatrix(getWorldTransform());
        }
    }

protected:
    void onRender(Renderer& renderer) override
    {
        syncToRenderer();
    }

private:
    Colour tint;
    RenderableMesh* mesh = nullptr;
};

class RotateCameraScene : public Scene {
public:
    void onInit(Renderer& renderer) override
    {
        camera.setPosition({0.0f, 0.0f});
        shader = renderer.newShader("shaders/2d_vert.glsl", "shaders/2d_frag.glsl");

        createQuad(renderer, {-1.25f, 0.0f, 0.0f}, Colour(220, 80, 80));
        createQuad(renderer, {0.0f, 0.0f, 0.0f}, Colour(240, 200, 90));
        createQuad(renderer, {1.25f, 0.0f, 0.0f}, Colour(80, 180, 220));
        createQuad(renderer, {0.0f, 1.15f, 0.0f}, Colour(110, 220, 140));
        createQuad(renderer, {0.0f, -1.15f, 0.0f}, Colour(170, 120, 230));
    }

    void onUpdate(float deltaTime) override
    {
        camera.handleInputs(deltaTime);
    }

    void onRender(Renderer& renderer) override
    {
        renderer.render(shader, camera);
    }

    void onShutdown() override
    {
        delete shader;
        shader = nullptr;
    }

private:
    void createQuad(Renderer& renderer, const glm::vec3& position, const Colour& tint)
    {
        ColouredQuadNode* quad = getRoot().createChild<ColouredQuadNode>(tint);
        quad->setPosition(position);
        quad->initRendering(renderer);
    }

    IShader* shader = nullptr;
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
