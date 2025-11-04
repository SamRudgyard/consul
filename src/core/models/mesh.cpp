#include "core/models/mesh.hpp"
#include "core/console/console.hpp"
#include "glad/glad.h"
#include "shader.hpp"
#include "core/models/texture.hpp"
#include "utils.hpp"
#include "core/camera/camera.hpp"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures) {
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &ebo);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);                 // Coordinates
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3*sizeof(float))); // Normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(6*sizeof(float))); // Colours
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(9*sizeof(float))); // Texture coordinates
    glEnableVertexAttribArray(3);

    // Unbind all to prevent accidentally modifying them
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glCheckError();
}

void Mesh::Draw(Shader& shader, Camera& camera, glm::mat4 matrix) const
{
    shader.Use();
    glBindVertexArray(vao);
    glCheckError();
    unsigned int numOfDiffuseTextures = 0;
    unsigned int numOfSpecularTextures = 0;
    for (unsigned int i = 0; i < textures.size(); i++) {
        std::string num = std::to_string(i);
        TextureType type = textures[i].type;
        if (type == TextureType::DIFFUSE) {
            num = std::to_string(numOfDiffuseTextures++);
        } else if (type == TextureType::SPECULAR) {
            num = std::to_string(numOfSpecularTextures++);
        }
        else {
            Console::Get().Error("Unknown texture type '" + std::to_string(type) + "'");
        }

        textures[i].SetTextureUnit(shader.id, (textures[i].GetTextureTypeAsString() + num).c_str(), i);
        textures[i].Bind();
    }

    shader.SetUniformVector3f("camPos", camera.position.x, camera.position.y, camera.position.z);

    camera.Matrix(shader, "camMatrix");

    glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);

    // Initialize matrices
	glm::mat4 trans = glm::mat4(1.0f);
	glm::mat4 rot = glm::mat4(1.0f);
	glm::mat4 sca = glm::mat4(1.0f);

	// Transform the matrices to their correct form
	trans = glm::translate(trans, translation);
	rot = glm::mat4_cast(rotation);
	sca = glm::scale(sca, scale);

    shader.SetUniformMatrix4f("translation", trans);
    shader.SetUniformMatrix4f("rotation", rot);
    shader.SetUniformMatrix4f("scale", sca);
    shader.SetUniformMatrix4f("model", matrix);

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}
