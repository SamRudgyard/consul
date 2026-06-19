#pragma once

#include "loader.hpp"

class ModelLoader : public Loader
{
public:
    ModelLoader(AssetManager& assetManager) : Loader(assetManager) {}

    std::vector<std::string> getSupportedExtensions() const override
    {
        return { ".gltf" };
    }

    /**
     * Load a model from the given file path.
     * @param filePath Path to the model file.
     */
    void load(const std::string& filePath) override;
protected:

};