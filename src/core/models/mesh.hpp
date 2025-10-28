#pragma once

#include "glm/glm.hpp"
#include <vector>

class Shader;
class Texture;

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

class Mesh
{
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    Mesh(const std::vector<Vertex>& vertices,
        const std::vector<unsigned int>& indices,
         const std::vector<Texture>& textures);
    void Draw(Shader& shader) const;

private:
    unsigned int vao, vbo, ebo;

    void setupMesh();

};