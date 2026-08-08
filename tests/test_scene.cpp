#include <catch2/catch_test_macros.hpp>

#include <memory>

#include "core/project/asset_defaults.hpp"
#include "core/project/asset_library.hpp"
#include "core/project/asset_manager.hpp"
#include "core/project/importers/gltf_importer.hpp"
#include "core/project/scene.hpp"
#include "core/project/scene_manager.hpp"
#include "graphics/models/model.hpp"

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
