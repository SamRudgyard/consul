#include "opengl_mesh.hpp"

#include "core/console/console.hpp"
#include "graphics/camera/camera.hpp"
#include "graphics/shaders/shader.hpp"
#include "graphics/textures/opengl/opengl_texture.hpp"
#include "glad/glad.h"
#include "utils.hpp"

OpenGLMesh::OpenGLMesh(Mesh& mesh)
    : RenderableMesh(mesh)
{
    glCheckError();

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glCheckError();

    if (mesh.getPositions().empty()) {
        Console::get().error("[OpenGLMesh::OpenGLMesh] Provided Mesh has no position data");
        return;
    }

    unsigned int positionVBO = enableVertexBuffer(mesh.getPositions(), AttributeType::POSITION, false);
    mesh.setVertexBuffer(positionVBO, AttributeType::POSITION);

    glCheckError();

    if (mesh.getNormals().empty()) {
        // Mesh doesn't have normals => use default normals
        glVertexAttrib3fv((unsigned int)(AttributeType::NORMAL), glm::value_ptr(glm::vec3(0.0f, 0.0f, 1.0f)));
    } else {
        unsigned int normalVBO = enableVertexBuffer(mesh.getNormals(), AttributeType::NORMAL, false);
        mesh.setVertexBuffer(normalVBO, AttributeType::NORMAL);
    }

    glCheckError();
    
    if (mesh.getColours().empty()) {
        // Mesh doesn't have colours => use default white colour
        glVertexAttrib4fv((unsigned int)(AttributeType::COLOUR), glm::value_ptr(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)));
    } else {
        unsigned int colourVBO = enableVertexBuffer(mesh.getColours(), AttributeType::COLOUR, false);
        mesh.setVertexBuffer(colourVBO, AttributeType::COLOUR);
    }

    glCheckError();

    if (mesh.getTextureCoords().empty()) {
        // Mesh doesn't have texture coordinates => use default (0, 0)
        glVertexAttrib2fv((unsigned int)(AttributeType::TEXCOORD), glm::value_ptr(glm::vec2(0.0f, 0.0f)));
    } else {
        unsigned int texCoordVBO = enableVertexBuffer(mesh.getTextureCoords(), AttributeType::TEXCOORD, false);
        mesh.setVertexBuffer(texCoordVBO, AttributeType::TEXCOORD);
    }

    glCheckError();

    if (mesh.getTangents().empty()) {
        // Mesh doesn't have tangents => use default tangent
        glVertexAttrib4fv((unsigned int)(AttributeType::TANGENT), glm::value_ptr(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)));
    } else {
        unsigned int tangentVBO = enableVertexBuffer(mesh.getTangents(), AttributeType::TANGENT, false);
        mesh.setVertexBuffer(tangentVBO, AttributeType::TANGENT);
    }

    glCheckError();

    // Generate elemnt buffer object (EBO) for indices
    const std::vector<unsigned int>& indices = mesh.getIndices();
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    mesh.setVertexBuffer(ebo, AttributeType::INDICES);

    // Unbind all to prevent accidental modification
    glBindVertexArray(0);                       // Unbind VAO first
    glBindBuffer(GL_ARRAY_BUFFER, 0);           // Then unbind VBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);   // Finally unbind EBO

    glCheckError();

    std::vector<Texture> meshTextures = mesh.getTextures();
    unsigned int numOfDiffuseTextures = 0;
    unsigned int numOfSpecularTextures = 0;
    for (unsigned int it = 0; it < meshTextures.size(); ++it) {
        const Texture& texture = meshTextures[it];
        std::string nTextureType;
        switch (texture.getType()) {
            case TextureType::DIFFUSE:
                nTextureType = std::to_string(numOfDiffuseTextures++);
                break;
            case TextureType::SPECULAR:
                nTextureType = std::to_string(numOfSpecularTextures++);
                break;
            default:
                Console::get().error("[OpenGLMesh::OpenGLMesh] Unsupported texture type");
                continue;
        }
        OpenGLTexture openglTexture(texture);
        textures.insert({texture.getTextureTypeAsString() + nTextureType, openglTexture});
    }
}

OpenGLMesh::~OpenGLMesh()
{
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &ebo);

    const Mesh& mesh = getMesh();
    
    unsigned int positionVBO = mesh.getVertexBuffer(AttributeType::POSITION);
    glDeleteBuffers(1, &positionVBO);
    
    unsigned int normalVBO = mesh.getVertexBuffer(AttributeType::NORMAL);
    if (normalVBO != 0) {
        glDeleteBuffers(1, &normalVBO);
    }

    unsigned int colourVBO = mesh.getVertexBuffer(AttributeType::COLOUR);
    if (colourVBO != 0) {
        glDeleteBuffers(1, &colourVBO);
    }

    unsigned int texCoordVBO = mesh.getVertexBuffer(AttributeType::TEXCOORD);
    if (texCoordVBO != 0) {
        glDeleteBuffers(1, &texCoordVBO);
    }

    unsigned int tangentVBO = mesh.getVertexBuffer(AttributeType::TANGENT);
    if (tangentVBO != 0) {
        glDeleteBuffers(1, &tangentVBO);
    }
}

unsigned int OpenGLMesh::enableVertexBuffer(const std::vector<glm::vec2>& data, AttributeType attribute, bool useDynamicDraw)
{
    unsigned int vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, data.size()*sizeof(glm::vec2), data.data(), useDynamicDraw ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

    glVertexAttribPointer((unsigned int)(attribute), 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray((unsigned int)(attribute));
    
    glCheckError();

    return vbo;
}

unsigned int OpenGLMesh::enableVertexBuffer(const std::vector<glm::vec3>& data, AttributeType attribute, bool useDynamicDraw)
{
    unsigned int vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, data.size()*sizeof(glm::vec3), data.data(), useDynamicDraw ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

    glVertexAttribPointer((unsigned int)(attribute), 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray((unsigned int)(attribute));

    glCheckError();

    return vbo;
}

unsigned int OpenGLMesh::enableVertexBuffer(const std::vector<glm::vec4>& data, AttributeType attribute, bool useDynamicDraw)
{
    unsigned int vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, data.size()*sizeof(glm::vec4), data.data(), useDynamicDraw ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

    glVertexAttribPointer((unsigned int)(attribute), 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray((unsigned int)(attribute));

    glCheckError();

    return vbo;
}

void OpenGLMesh::draw(const IShader* shader, const Camera& camera) const
{
    shader->use();
    glBindVertexArray(vao);
    glCheckError();

    for (const auto& [name, texture] : textures) {
        texture.bind();
        texture.setTextureUnit(shader, name.c_str());
        glCheckError();
    }

    glCheckError();
    shader->use();

    shader->setUniformVec3("lightPosition", 0.0f, 0.0f, 20.0f);
    glCheckError();
    shader->setUniformVec3("lightColour", 1.0f, 1.0f, 1.0f);
    glCheckError();
    shader->setUniformMat4("model", getModelMatrix());
    glCheckError();

    camera.sendToShader(shader);
    glCheckError();

    glDrawElements(GL_TRIANGLES, this->getMesh().getNumIndices(), GL_UNSIGNED_INT, 0);
    glCheckError();
}