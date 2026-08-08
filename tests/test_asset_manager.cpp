#include <catch2/catch_test_macros.hpp>

#include <algorithm>
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
    manager.removeExpiredAssets();
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

TEST_CASE("a preserved asset remains alive without external references")
{
    AssetManager<TestAsset> manager;
    std::shared_ptr<TestAsset> asset = manager.add("Test Asset", TestAsset{});
    std::weak_ptr<TestAsset> assetObserver = asset;

    manager.setSourcePath(asset, "assets/test.asset");
    manager.setPreserved(asset, true);
    REQUIRE(manager.isPreserved(asset));

    asset.reset();

    REQUIRE_FALSE(assetObserver.expired());
    const std::vector<std::shared_ptr<TestAsset>> assets = manager.getAssets();
    REQUIRE(assets.size() == 1);

    const std::optional<AssetMetadata> metadata = manager.getMetadata(assets.front());
    REQUIRE(metadata.has_value());
    REQUIRE(metadata->name == "Test Asset");
    REQUIRE(metadata->sourcePath == "assets/test.asset");
}

TEST_CASE("an asset manager returns non-owning and preserved assets")
{
    AssetManager<TestAsset> manager;
    std::shared_ptr<TestAsset> nonOwningAsset = manager.add("Non-owning Asset", TestAsset{});
    std::shared_ptr<TestAsset> preservedAsset = manager.add("Preserved Asset", TestAsset{});

    manager.setPreserved(preservedAsset, true);

    const std::vector<std::shared_ptr<TestAsset>> assets = manager.getAssets();
    REQUIRE(assets.size() == 2);
    REQUIRE(std::find(assets.begin(), assets.end(), nonOwningAsset) != assets.end());
    REQUIRE(std::find(assets.begin(), assets.end(), preservedAsset) != assets.end());
}

TEST_CASE("removing preservation from a preserved asset releases it")
{
    AssetManager<TestAsset> manager;
    std::shared_ptr<TestAsset> asset = manager.add("Test Asset", TestAsset{});
    std::weak_ptr<TestAsset> assetObserver = asset;

    manager.setPreserved(asset, true);
    asset.reset();

    std::shared_ptr<TestAsset> retainedAsset = manager.getAssets().front();
    manager.setPreserved(retainedAsset, false);
    REQUIRE_FALSE(manager.isPreserved(retainedAsset));

    retainedAsset.reset();

    REQUIRE(assetObserver.expired());
    REQUIRE(manager.getAssets().empty());
}
