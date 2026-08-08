#include <catch2/catch_test_macros.hpp>

#include <memory>

#include "core/project/asset_manager.hpp"

namespace
{
    struct TestAsset
    {
    };
}

TEST_CASE("an asset manager does not own registered assets")
{
    AssetManager<TestAsset> manager;
    std::shared_ptr<TestAsset> asset = manager.add("Test Asset", TestAsset{});
    std::weak_ptr<TestAsset> assetObserver = asset;

    REQUIRE(manager.getAssets().size() == 1);

    asset.reset();

    REQUIRE(assetObserver.expired());
    REQUIRE(manager.getAssets().empty());
}

TEST_CASE("an asset manager retains metadata for live assets")
{
    AssetManager<TestAsset> manager;
    std::shared_ptr<TestAsset> asset = manager.add("Test Asset", TestAsset{});

    manager.setSourcePath(asset, "assets/test.asset");

    const std::optional<AssetMetadata> metadata = manager.getMetadata(asset);
    REQUIRE(metadata.has_value());
    REQUIRE(metadata->name == "Test Asset");
    REQUIRE(metadata->sourcePath == "assets/test.asset");
}
