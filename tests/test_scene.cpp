#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include <memory>

#include "core/ecs/components.hpp"
#include "core/engine.hpp"
#include "core/project/scene.hpp"
#include "core/project/scene_manager.hpp"
#include "graphics/camera/camera.hpp"
#include "graphics/material/material.hpp"
#include "graphics/mesh/mesh.hpp"
#include "graphics/models/model.hpp"
#include "graphics/renderer/renderer.hpp"
#include "graphics/shader/shader.hpp"

namespace
{

    class ModelOwningScene : public Scene
    {
    public:
        explicit ModelOwningScene(std::weak_ptr<Model>& modelObserver)
            : modelObserver(modelObserver)
        {
        }

    protected:
        void onInit() override
        {
            std::shared_ptr<Model> model = Engine::get().getModelAssetManager()->add("Scene Model", Model{});
            modelObserver = model;

            const Entity entity = getECS().createEntity();
            getECS().addComponent<ModelRenderer>(entity, ModelRenderer{std::move(model), true});
        }

    private:
        std::weak_ptr<Model>& modelObserver;
    };

    class TestCamera : public Camera
    {
    public:
        void handleInputs(double) override {}
        glm::vec3 getPosition() const override { return glm::vec3(0.0f); }
        glm::mat4 getCameraMatrix() const override { return glm::mat4(1.0f); }
    };

    class RenderableScene : public Scene
    {
    public:
        Camera* getActiveCamera() override { return &camera; }

        const ModelPrimitive& getPrimitive() const
        {
            return getECS().getComponent<ModelRenderer>(renderEntity).model->getPrimitives().front();
        }

        void setVisible(bool visible)
        {
            getECS().getComponent<ModelRenderer>(renderEntity).visible = visible;
        }

    protected:
        void onInit() override
        {
            Engine& engine = Engine::get();

            std::shared_ptr<VertexShader> vertexShader = engine.getVertexShaderAssetManager()->add(
                "Test Vertex Shader",
                VertexShader("vertex source")
            );
            std::shared_ptr<FragmentShader> fragmentShader = engine.getFragmentShaderAssetManager()->add(
                "Test Fragment Shader",
                FragmentShader("fragment source")
            );
            shader = engine.getShaderAssetManager()->add(
                "Test Shader",
                Shader(std::move(vertexShader), std::move(fragmentShader))
            );

            Material material;
            std::shared_ptr<Material> materialAsset = engine.getMaterialAssetManager()->add(
                "Test Material",
                engine.getAssetDefaults().applyToMaterial(material)
            );

            Mesh meshData;
            std::shared_ptr<Mesh> mesh = engine.getMeshAssetManager()->add("Test Mesh", meshData);
            glm::mat4 localTransform(1.0f);
            localTransform[3][0] = 1.0f;
            localTransform[3][1] = 2.0f;

            Model modelData;
            modelData.addPrimitive(std::move(mesh), std::move(materialAsset), localTransform);
            std::shared_ptr<Model> model = engine.getModelAssetManager()->add("Test Model", modelData);
            unreferencedModel = engine.getModelAssetManager()->add("Unreferenced Test Model", modelData);

            Transform transform;
            transform.position = {3.0f, 4.0f, 0.0f};
            transform.rotation.z = glm::radians(90.0f);
            transform.scale = {2.0f, 3.0f, 1.0f};

            renderEntity = getECS().createEntity();
            getECS().addComponent<Transform>(renderEntity, transform);
            getECS().addComponent<ModelRenderer>(renderEntity, ModelRenderer{model, true});

            const Entity hiddenEntity = getECS().createEntity();
            getECS().addComponent<Transform>(hiddenEntity);
            getECS().addComponent<ModelRenderer>(hiddenEntity, ModelRenderer{std::move(model), false});
        }

    private:
        TestCamera camera;
        std::shared_ptr<Shader> shader;
        std::shared_ptr<Model> unreferencedModel;
        Entity renderEntity = 0;
    };

    class CapturingRenderer : public Renderer
    {
    public:
        void initialiseGraphics(void*) override {}
        void initialiseImGui() override {}
        void clearBackground(const glm::vec4&) override {}
        void clearScreenBuffer() override {}
        void setViewport(int, int, int, int) override {}
        void uploadShader(const std::shared_ptr<Shader>&) override {}
        void uploadMesh(const std::shared_ptr<Mesh>&) override {}
        void uploadTexture(const std::shared_ptr<Texture>&) override {}
        void releaseExpiredResources() override {}

        void render(
            const std::shared_ptr<Shader>&,
            const Camera&,
            const std::vector<RenderItem>& renderItems
        ) override
        {
            submittedItems = renderItems;
        }

        std::vector<RenderItem> submittedItems;
    };
}

TEST_CASE("destroying an uninitialised scene is safe")
{
    Scene scene;
}

TEST_CASE("scenes own independent ECS worlds")
{
    Scene firstScene;
    Scene secondScene;

    const Entity entity = firstScene.getECS().createEntity();

    REQUIRE(firstScene.getECS().isAlive(entity));
    REQUIRE(firstScene.getECS().getEntityCount() == 1);
    REQUIRE(secondScene.getECS().getEntityCount() == 0);

    const Scene& constScene = firstScene;
    REQUIRE(constScene.getECS().isAlive(entity));
}

TEST_CASE("unloading a scene preserves assets retained by another owner")
{
    Engine& engine = Engine::get();
    SceneManager* sceneManager = engine.getSceneManager();
    ModelAssetManager* modelManager = engine.getModelAssetManager();

    std::weak_ptr<Model> modelObserver;
    sceneManager->loadScene(std::make_unique<ModelOwningScene>(modelObserver));
    std::shared_ptr<Model> retainedModel = modelManager->get("Scene Model");

    sceneManager->unloadScene();

    REQUIRE_FALSE(modelObserver.expired());
    REQUIRE(modelManager->get("Scene Model") == retainedModel);

    retainedModel.reset();

    REQUIRE(modelObserver.expired());
    REQUIRE_FALSE(modelManager->get("Scene Model"));
}

TEST_CASE("shutting down a scene releases its unshared assets")
{
    Engine& engine = Engine::get();
    SceneManager* sceneManager = engine.getSceneManager();
    ModelAssetManager* modelManager = engine.getModelAssetManager();

    std::weak_ptr<Model> modelObserver;
    sceneManager->loadScene(std::make_unique<ModelOwningScene>(modelObserver));

    sceneManager->shutdown();

    REQUIRE_FALSE(sceneManager->hasScene());
    REQUIRE(modelObserver.expired());
    REQUIRE_FALSE(modelManager->get("Scene Model"));
}

TEST_CASE("scene rendering submits active model primitives to the renderer")
{
    Engine& engine = Engine::get();
    SceneManager* sceneManager = engine.getSceneManager();
    ModelAssetManager* modelManager = engine.getModelAssetManager();

    auto scene = std::make_unique<RenderableScene>();
    RenderableScene* scenePointer = scene.get();
    sceneManager->loadScene(std::move(scene));
    REQUIRE(modelManager->get("Test Model"));
    REQUIRE(modelManager->get("Unreferenced Test Model"));

    CapturingRenderer renderer;
    sceneManager->render(renderer);

    REQUIRE(renderer.submittedItems.size() == 1);
    REQUIRE(renderer.submittedItems.front().mesh == scenePointer->getPrimitive().mesh);
    REQUIRE(renderer.submittedItems.front().material == scenePointer->getPrimitive().material);
    REQUIRE(renderer.submittedItems.front().modelMatrix[3][0] == Catch::Approx(-3.0f));
    REQUIRE(renderer.submittedItems.front().modelMatrix[3][1] == Catch::Approx(6.0f));

    scenePointer->setVisible(false);
    sceneManager->render(renderer);

    REQUIRE(renderer.submittedItems.empty());
}
