#pragma once

#include <string>
#include <vector>

#include "core/project/asset_manager.hpp"
#include "utils.hpp"

class Loader
{
public:
    Loader() = default;
    virtual ~Loader() = default;

    void setAssetManager(std::shared_ptr<AssetManager> assetManager)
    {
        this->assetManager = assetManager;
    }

    virtual std::vector<std::string> getSupportedExtensions() const = 0;

    bool isSupportedExtension(const std::string& extension) const
    {
        return contains(getSupportedExtensions(), extension);
    }

    /**
     * Load an asset from the given file path.
     * @param filePath Path to the asset file.
     */
    virtual void load(const std::string& filePath) = 0;
protected:
    std::weak_ptr<AssetManager> assetManager;
};