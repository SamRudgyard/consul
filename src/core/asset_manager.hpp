#pragma once

class Mesh;
class Model;
class Shader;
class Texture;

#include <string>
#include <vector>

class AssetManager
{
public:
    AssetManager() = default;
    ~AssetManager() = default;

    Mesh* loadMesh(Mesh& mesh);
    Model* loadModel(Model& model);
    Shader* loadShader(Shader& shader);
    Texture* loadTexture(Texture& texture);

private:
    std::vector<Mesh> meshes;
    std::vector<Model> models;
    std::vector<Texture> textures;
    std::vector<Shader> shaders;
};