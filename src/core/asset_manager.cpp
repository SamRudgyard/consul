#include "asset_manager.hpp"

#include "graphics/mesh/mesh.hpp"
#include "graphics/models/model.hpp"
#include "graphics/shader/shader.hpp"
#include "graphics/texture/texture.hpp"

Mesh* AssetManager::loadMesh(Mesh& mesh)
{
    meshes.push_back(mesh);
    return &meshes.back();
}

Model* AssetManager::loadModel(Model& model)
{
    models.push_back(model);
    return &models.back();
}

Shader* AssetManager::loadShader(Shader& shader)
{
    shaders.push_back(shader);
    return &shaders.back();
}

Texture* AssetManager::loadTexture(Texture& texture)
{
    textures.push_back(texture);
    return &textures.back();
}

