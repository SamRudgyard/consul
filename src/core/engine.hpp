#pragma once

#include "core/project/asset_manager.hpp"
#include "core/project/scene_manager.hpp"
#include "core/time.hpp"
#include "core/window.hpp"
#include "input/input_system.hpp"

class Engine
{
public:
    Engine() = default;
    ~Engine() = default;

    Engine(const Engine&) = delete; // Delete copy constructor
    Engine& operator=(const Engine&) = delete; // Delete copy assignment operator

    Window window;
    Time time;
    InputSystem inputSystem;

    MeshAssetManager* getMeshAssetManager() { return &meshManager; }
    ModelAssetManager* getModelAssetManager() { return &modelManager; }
    MaterialAssetManager* getMaterialAssetManager() { return &materialManager; }
    ShaderAssetManager* getShaderAssetManager() { return &shaderManager; }
    VertexShaderAssetManager* getVertexShaderAssetManager() { return &vertexShaderManager; }
    FragmentShaderAssetManager* getFragmentShaderAssetManager() { return &fragmentShaderManager; }
    TextureAssetManager* getTextureAssetManager() { return &textureManager; }

    SceneManager* getSceneManager() { return &sceneManager; }

private:
    MeshAssetManager meshManager;
    ModelAssetManager modelManager;
    MaterialAssetManager materialManager;
    ShaderAssetManager shaderManager;
    VertexShaderAssetManager vertexShaderManager;
    FragmentShaderAssetManager fragmentShaderManager;
    TextureAssetManager textureManager;
    SceneManager sceneManager;
};
