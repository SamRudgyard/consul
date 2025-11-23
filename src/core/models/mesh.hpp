#pragma once

#include "glm/glm.hpp"
#include "core/camera/camera.hpp"
#include "core/models/texture.hpp"

#include <string>
#include <vector>
#include <iostream>

class Shader;

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 colour;
    glm::vec2 textureCoords;
};

class Mesh {
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    unsigned int vao;
    unsigned int vbo;
    unsigned int ebo;

    Mesh() {};
    
    /**
     * Construct a mesh from vertex and index data, and any associated textures.
     * @param vertices Vertex attribute data.
     * @param indices  Element index data.
     * @param textures Associated textures.
     */
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);

    /**
     * Draws the mesh.
     * @param shader               Shader program to use for drawing.
     * @param camera               Camera providing view/projection matrices.
     * @param transformationMatrix Local transformation matrix for the mesh.
     * @param translation          Additional translation to apply.
     * @param rotation             Additional rotation to apply.
     * @param scale                Additional scale to apply.
     */
    void Draw(Shader& shader, Camera& camera, glm::mat4 transformationMatrix = glm::mat4(1.0f), glm::vec3 translation = glm::vec3(0.0f), glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3 scale = glm::vec3(1.0f)) const;
};