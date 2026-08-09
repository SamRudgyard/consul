#include <catch2/catch_test_macros.hpp>

#include <memory>

#include "core/project/asset_defaults.hpp"
#include "core/project/asset_library.hpp"
#include "core/project/asset_manager.hpp"
#include "core/project/importers/gltf_importer.hpp"
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
    std::shared_ptr<AssetLibrary> makeAssetLibrary()
    {
        auto modelManager = std::make_shared<ModelAssetManager>();
        auto meshManager = std::make_shared<MeshAssetManager>();
        auto materialManager = std::make_shared<MaterialAssetManager>();
        auto textureManager = std::make_shared<TextureAssetManager>();
        auto shaderManager = std::make_shared<ShaderAssetManager>();
        auto vertexShaderManager = std::make_shared<VertexShaderAssetManager>();
        auto fragmentShaderManager = std::make_shared<FragmentShaderAssetManager>();
        auto defaults = std::make_shared<AssetDefaults>(materialManager, textureManager);
        auto importer = std::make_shared<GLTFImporter>(modelManager, meshManager, materialManager, textureManager, defaults);

        return std::make_shared<AssetLibrary>(
            modelManager,
            meshManager,
            materialManager,
            textureManager,
            shaderManager,
            vertexShaderManager,
            fragmentShaderManager,
            defaults,
            importer
        );
    }

    class ModelOwningScene : public Scene
    {
    public:
        explicit ModelOwningScene(std::weak_ptr<Model>& modelObserver)
            : modelObserver(modelObserver)
        {
        }

    protected:
        void onInit(std::shared_ptr<AssetLibrary> assets) override
        {
            model = assets->addModel("Scene Model", Model{});
            modelObserver = model;
        }

    private:
        std::weak_ptr<Model>& modelObserver;
        std::shared_ptr<Model> model;
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

        std::shared_ptr<Mesh> getMesh() const { return mesh; }

    protected:
        void onInit(std::shared_ptr<AssetLibrary> assets) override
        {
            std::shared_ptr<VertexShader> vertexShader = assets->addVertexShader("Test Vertex Shader", VertexShader("vertex source"));
            std::shared_ptr<FragmentShader> fragmentShader = assets->addFragmentShader("Test Fragment Shader", FragmentShader("fragment source"));
            shader = assets->addShader("Test Shader", Shader(std::move(vertexShader), std::move(fragmentShader)));

            Material material;
            std::shared_ptr<Material> materialAsset = assets->addMaterial("Test Material", material);

            Mesh meshData;
            meshData.setMaterial(std::move(materialAsset));
            glm::mat4 modelMatrix(1.0f);
            modelMatrix[3][0] = 3.0f;
            modelMatrix[3][1] = 4.0f;
            meshData.setModelMatrix(modelMatrix);
            mesh = assets->addMesh("Test Mesh", meshData);
        }

    private:
        TestCamera camera;
        std::shared_ptr<Shader> shader;
        std::shared_ptr<Mesh> mesh;
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

TEST_CASE("unloading a scene preserves assets retained by another owner")
{
    std::shared_ptr<AssetLibrary> assets = makeAssetLibrary();
    SceneManager scenes;
    scenes.assignAssets(assets);
    std::weak_ptr<Model> modelObserver;
    scenes.loadScene(std::make_unique<ModelOwningScene>(modelObserver));
    std::shared_ptr<Model> retainedModel = assets->getModels().front();

    scenes.unloadScene();

    REQUIRE_FALSE(modelObserver.expired());
    REQUIRE(assets->getModels().size() == 1);

    retainedModel.reset();

    REQUIRE(modelObserver.expired());
    REQUIRE(assets->getModels().empty());
}

TEST_CASE("shutting down a scene releases its unshared assets")
{
    std::shared_ptr<AssetLibrary> assets = makeAssetLibrary();
    SceneManager scenes;
    scenes.assignAssets(assets);
    std::weak_ptr<Model> modelObserver;
    scenes.loadScene(std::make_unique<ModelOwningScene>(modelObserver));

    scenes.shutdown();

    REQUIRE_FALSE(scenes.hasScene());
    REQUIRE(modelObserver.expired());
    REQUIRE(assets->getModels().empty());
}

TEST_CASE("scene rendering submits active mesh state to the renderer")
{
    std::shared_ptr<AssetLibrary> assets = makeAssetLibrary();
    SceneManager scenes;
    scenes.assignAssets(assets);

    auto scene = std::make_unique<RenderableScene>();
    RenderableScene* scenePointer = scene.get();
    scenes.loadScene(std::move(scene));

    CapturingRenderer renderer;
    scenes.render(renderer);

    REQUIRE(renderer.submittedItems.size() == 1);
    REQUIRE(renderer.submittedItems.front().mesh == scenePointer->getMesh());
    REQUIRE(renderer.submittedItems.front().material == scenePointer->getMesh()->getMaterial());
    REQUIRE(renderer.submittedItems.front().modelMatrix[3][0] == 3.0f);
    REQUIRE(renderer.submittedItems.front().modelMatrix[3][1] == 4.0f);
}
