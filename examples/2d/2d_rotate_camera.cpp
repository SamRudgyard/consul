#include "core/consul.hpp"
#include "core/project/asset_manager.hpp"
#include "core/project/scene.hpp"
#include "graphics/camera/camera_2d.hpp"
#include "graphics/colour.hpp"
#include "graphics/geometry/geometry_2d.hpp"
#include "graphics/material/material.hpp"
#include "graphics/models/model.hpp"
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
    void initialise(std::shared_ptr<AssetManager> assetManager, const Colour& tint)
    {
        Mesh mesh = Geometry2D::get()->rect({-0.35f, -0.35f}, {0.35f, 0.35f});

        std::shared_ptr<Material> material = std::make_shared<Material>();
        material->setAlbedo(tint);
        assetManager->addMaterial("quadMaterial", material);

        mesh.setMaterial(material);

        std::shared_ptr<Model> model = std::make_shared<Model>();
        model->addMesh(mesh);
        modelID = assetManager->addModel("quadModel", model);
    }

protected:
    void onUpdate(std::shared_ptr<AssetManager> assetManager, double deltaTime) override
    {
        std::shared_ptr<Model> model = assetManager->getModel(modelID);
        if (!model) {
            return;
        }

        model->setTransform(getLocalTransform());
    }

private:
    AssetID modelID;
};

class RotateCameraScene : public Scene {
public:
    void onInit(std::shared_ptr<AssetManager> assetManager) override
    {
        camera.setPosition({0.0f, 0.0f});
        assetManager->addShader("default", std::make_shared<Shader>("shaders/default_vertex_2d.glsl", "shaders/default_fragment_2d.glsl"));

        createQuad(assetManager, {-1.25f, 0.0f, 0.0f}, Colour(220, 80, 80));
        createQuad(assetManager, {0.0f, 0.0f, 0.0f}, Colour(240, 200, 90));
        createQuad(assetManager, {1.25f, 0.0f, 0.0f}, Colour(80, 180, 220));
        createQuad(assetManager, {0.0f, 1.15f, 0.0f}, Colour(110, 220, 140));
        createQuad(assetManager, {0.0f, -1.15f, 0.0f}, Colour(170, 120, 230));
    }

    void onUpdate(std::shared_ptr<AssetManager> assetManager, double deltaTime) override
    {
        camera.handleInputs(deltaTime);
    }

    Camera* getActiveCamera() override { return &camera; }

private:
    void createQuad(std::shared_ptr<AssetManager> assetManager, const glm::vec3& position, const Colour& tint)
    {
        ColouredQuadNode* quad = getRoot().createChild<ColouredQuadNode>();
        quad->setPosition(position);
        quad->initialise(assetManager, tint);
    }

    RotatingCamera2D camera;
};

int main()
{
    Window window;
    window.title = "2D Example - Custom Rotating Camera";

    Consul consul(window);
    consul.setTargetFPS(60);
    RotateCameraScene scene;
    consul.loadScene(scene);
    consul.run();
    return 0;
}
