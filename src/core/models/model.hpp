#pragma once

#include "glm/glm.hpp"
#include "core/models/mesh.hpp"
#include "core/models/texture.hpp"
#include "core/camera/camera.hpp"
#include <string>
#include <vector>

class aiNode;
class aiScene;
class aiMesh;
class aiMaterial;
class Shader;

class Model
{
public:
    Model(const std::string& filePath, glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f)) : scale(scale) { loadModel(filePath); }
    Model(std::vector<Mesh> meshes, std::vector<Texture> textures) : meshes(std::move(meshes)), loadedTextures(std::move(textures)) {}
    void Draw(Shader& shader, Camera& camera) const;
private:
    std::vector<Mesh> meshes;
    std::vector<Texture> loadedTextures;
    std::string directory;
    glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);

    void loadModel(const std::string& filePath);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, TextureType type);
};