#include "core/engine.hpp"

#include <memory>

#include "core/project/asset_defaults.hpp"
#include "core/project/importers/gltf_importer.hpp"

Engine::Engine()
    : assetDefaults(std::make_unique<AssetDefaults>(materialManager, textureManager)),
      gltfImporter(std::make_unique<GLTFImporter>(
          modelManager,
          meshManager,
          materialManager,
          textureManager,
          *assetDefaults
      ))
{
}

Engine::~Engine() = default;
