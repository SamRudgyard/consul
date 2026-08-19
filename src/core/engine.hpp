#pragma once

#include <memory>

#include "core/profiling/profiler.hpp"
#include "core/project/asset_defaults.hpp"
#include "core/project/asset_manager.hpp"
#include "core/project/importers/gltf_importer.hpp"
#include "core/project/scene_manager.hpp"
#include "core/time.hpp"
#include "core/window.hpp"
#include "input/input_system.hpp"

class AssetDefaults;
class GLTFImporter;

class Engine
{
public:

    static Engine& get()
    {
        static Engine instance;
        return instance;
    }

    Engine(const Engine&) = delete; // Delete copy constructor
    Engine& operator=(const Engine&) = delete; // Delete copy assignment operator

    Window window;
    Time time;
    InputSystem inputSystem;
    Profiler profiler;

    MeshAssetManager* getMeshAssetManager() { return &meshManager; }
    ModelAssetManager* getModelAssetManager() { return &modelManager; }
    MaterialAssetManager* getMaterialAssetManager() { return &materialManager; }
    ShaderAssetManager* getShaderAssetManager() { return &shaderManager; }
    VertexShaderAssetManager* getVertexShaderAssetManager() { return &vertexShaderManager; }
    FragmentShaderAssetManager* getFragmentShaderAssetManager() { return &fragmentShaderManager; }
    TextureAssetManager* getTextureAssetManager() { return &textureManager; }

    AssetDefaults& getAssetDefaults() { return assetDefaults; }
    GLTFImporter& getGLTFImporter() { return gltfImporter; }
    SceneManager* getSceneManager() { return &sceneManager; }

private:
    Engine() = default;
    ~Engine() = default;

    MeshAssetManager meshManager;
    ModelAssetManager modelManager;
    MaterialAssetManager materialManager;
    ShaderAssetManager shaderManager;
    VertexShaderAssetManager vertexShaderManager;
    FragmentShaderAssetManager fragmentShaderManager;
    TextureAssetManager textureManager;

    AssetDefaults assetDefaults = AssetDefaults(materialManager, textureManager);
    GLTFImporter gltfImporter = GLTFImporter(modelManager, meshManager, materialManager, textureManager, assetDefaults);

    SceneManager sceneManager;
};
