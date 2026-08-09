#include <memory>
#include <utility>

#include "core/consul.hpp"
#include "core/project/asset_library.hpp"
#include "core/project/scene.hpp"
#include "graphics/camera/camera_2d.hpp"
#include "graphics/colour.hpp"
#include "graphics/geometry/geometry_2d.hpp"
#include "graphics/material/material.hpp"
#include "graphics/models/model.hpp"

class RotatingCamera2D : public Camera2D {
public:
    void handleInputs(double deltaTime) override
    {
        InputSystem& input = Engine::get()->inputSystem;

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
    void initialise(std::shared_ptr<AssetLibrary> assets, const Colour& tint)
    {
        Mesh mesh = Geometry2D::get()->rect({-0.35f, -0.35f}, {0.35f, 0.35f});

        Material material;
        material.setAlbedo(tint);
        std::shared_ptr<Material> materialAsset = assets->addMaterial("quadMaterial", material);

        mesh.setMaterial(std::move(materialAsset));
        std::shared_ptr<Mesh> meshAsset = assets->addMesh("quadMesh", mesh);

        Model model;
        model.addMesh(std::move(meshAsset));
        modelAsset = assets->addModel("quadModel", model);
    }

protected:
    void onUpdate(std::shared_ptr<AssetLibrary> assets, double deltaTime) override
    {
        if (!modelAsset) {
            return;
        }

        modelAsset->setTransform(getLocalTransform());
    }

private:
    std::shared_ptr<Model> modelAsset;
};

class RotateCameraScene : public Scene {
public:
    void onInit(std::shared_ptr<AssetLibrary> assets) override
    {
        camera.setPosition({0.0f, 0.0f});
        defaultShader = assets->importShader("default", "shaders/default_vertex_2d.glsl", "shaders/default_fragment_2d.glsl");

        createQuad(assets, {-1.25f, 0.0f, 0.0f}, Colour(220, 80, 80));
        createQuad(assets, {0.0f, 0.0f, 0.0f}, Colour(240, 200, 90));
        createQuad(assets, {1.25f, 0.0f, 0.0f}, Colour(80, 180, 220));
        createQuad(assets, {0.0f, 1.15f, 0.0f}, Colour(110, 220, 140));
        createQuad(assets, {0.0f, -1.15f, 0.0f}, Colour(170, 120, 230));
    }

    void onUpdate(std::shared_ptr<AssetLibrary> assets, double deltaTime) override
    {
        camera.handleInputs(deltaTime);
    }

    Camera* getActiveCamera() override { return &camera; }

private:
    void createQuad(std::shared_ptr<AssetLibrary> assets, const glm::vec3& position, const Colour& tint)
    {
        ColouredQuadNode* quad = getRoot().createChild<ColouredQuadNode>();
        quad->setPosition(position);
        quad->initialise(assets, tint);
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
