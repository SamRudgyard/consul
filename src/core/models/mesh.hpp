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
        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);

        void Draw(Shader& shader, Camera& camera, glm::mat4 matrix = glm::mat4(1.0f)) const;
};