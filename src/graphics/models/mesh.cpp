#include "core/models/mesh.hpp"
#include "core/console/console.hpp"
#include "glad/glad.h"
#include "core/models/texture.hpp"
#include "utils.hpp"
#include "graphics/camera/camera.hpp"

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

void Mesh::draw(Shader& shader, Camera& camera, glm::mat4 transformationMatrix, glm::vec3 translation, glm::quat rotation, glm::vec3 scale) const
{
    shader.use();
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
            Console::get().error("[Mesh::draw] Unknown texture type '" + std::to_string(type) + "'");
        }

        textures[i].bind();
        textures[i].setTextureUnit(shader.id, (textures[i].getTextureTypeAsString() + num).c_str());
    }

    shader.setUniformVector3f("cameraPosition", camera.position.x, camera.position.y, camera.position.z);
    shader.setUniformVector3f("lightPosition", 0.0f, 0.0f, 20.0f);
    shader.setUniformVector3f("lightColour", 1.0f, 1.0f, 1.0f);

    camera.useCameraMatrix(shader, "camMatrix");

    glm::mat4 s = glm::scale(glm::mat4(1.0f), scale);
    glm::mat4 r = glm::mat4_cast(rotation);
    glm::mat4 t = glm::translate(glm::mat4(1.0f), translation);

    shader.setUniformMatrix4f("model", s*r*t*transformationMatrix);

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}
