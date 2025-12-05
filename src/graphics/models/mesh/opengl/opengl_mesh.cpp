#include "opengl_mesh.hpp"

#include "core/console/console.hpp"
#include "graphics/camera/camera.hpp"
#include "graphics/shaders/shader.hpp"
#include "glad/glad.h"
#include "utils.hpp"

OpenGLMesh::OpenGLMesh(MeshData& meshData)
    : IMesh(meshData)
{
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    if (meshData.getPositions().empty()) {
        Console::get().error("[OpenGLMesh::OpenGLMesh] Provided MeshData has no position data");
        return;
    }

    unsigned int positionVBO = enableVertexBuffer(meshData.getPositions(), AttributeType::POSITION, false);
    meshData.setVertexBuffer(positionVBO, AttributeType::POSITION);

    if (meshData.getNormals().empty()) {
        // Mesh doesn't have normals => use default normals
        glVertexAttrib3fv((unsigned int)(AttributeType::NORMAL), glm::value_ptr(glm::vec3(0.0f, 0.0f, 1.0f)));
    } else {
        unsigned int normalVBO = enableVertexBuffer(meshData.getNormals(), AttributeType::NORMAL, false);
        meshData.setVertexBuffer(normalVBO, AttributeType::NORMAL);
    }
    
    if (meshData.getColours().empty()) {
        // Mesh doesn't have colours => use default white colour
        glVertexAttrib4fv((unsigned int)(AttributeType::COLOUR), glm::value_ptr(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)));
    } else {
        unsigned int colourVBO = enableVertexBuffer(meshData.getColours(), AttributeType::COLOUR, false);
        meshData.setVertexBuffer(colourVBO, AttributeType::COLOUR);
    }

    if (meshData.getTextureCoords().empty()) {
        // Mesh doesn't have texture coordinates => use default (0, 0)
        glVertexAttrib2fv((unsigned int)(AttributeType::TEXCOORD), glm::value_ptr(glm::vec2(0.0f, 0.0f)));
    } else {
        unsigned int texCoordVBO = enableVertexBuffer(meshData.getTextureCoords(), AttributeType::TEXCOORD, false);
        meshData.setVertexBuffer(texCoordVBO, AttributeType::TEXCOORD);
    }

    if (meshData.getTangents().empty()) {
        // Mesh doesn't have tangents => use default tangent
        glVertexAttrib4fv((unsigned int)(AttributeType::TANGENT), glm::value_ptr(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)));
    } else {
        unsigned int tangentVBO = enableVertexBuffer(meshData.getTangents(), AttributeType::TANGENT, false);
        meshData.setVertexBuffer(tangentVBO, AttributeType::TANGENT);
    }

    // Generate elemnt buffer object (EBO) for indices
    const std::vector<unsigned int>& indices = meshData.getIndices();
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    meshData.setVertexBuffer(ebo, AttributeType::INDICES);

    // Unbind all to prevent accidental modification
    glBindVertexArray(0);                       // Unbind VAO first
    glBindBuffer(GL_ARRAY_BUFFER, 0);           // Then unbind VBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);   // Finally unbind EBO

    glCheckError();
}

OpenGLMesh::~OpenGLMesh()
{
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &ebo);

    const MeshData& meshData = getMeshData();
    
    unsigned int positionVBO = meshData.getVertexBuffer(AttributeType::POSITION);
    glDeleteBuffers(1, &positionVBO);
    
    unsigned int normalVBO = meshData.getVertexBuffer(AttributeType::NORMAL);
    if (normalVBO != 0) {
        glDeleteBuffers(1, &normalVBO);
    }

    unsigned int colourVBO = meshData.getVertexBuffer(AttributeType::COLOUR);
    if (colourVBO != 0) {
        glDeleteBuffers(1, &colourVBO);
    }

    unsigned int texCoordVBO = meshData.getVertexBuffer(AttributeType::TEXCOORD);
    if (texCoordVBO != 0) {
        glDeleteBuffers(1, &texCoordVBO);
    }

    unsigned int tangentVBO = meshData.getVertexBuffer(AttributeType::TANGENT);
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

    return vbo;
}

void OpenGLMesh::draw(IShader* shader, Camera& camera) const
{
    shader->use();
    glBindVertexArray(vao);
    glCheckError();

    const MeshData& meshData = getMeshData();
    unsigned int numOfDiffuseTextures = 0;
    unsigned int numOfSpecularTextures = 0;
    for (unsigned int it = 0; it < meshData.getTextures().size(); ++it) {
        std::string num = std::to_string(it);
        ITexture* texture = meshData.getTextures()[it];
        TextureType type = texture->getTextureType();
        if (type == TextureType::DIFFUSE) {
            num = std::to_string(numOfDiffuseTextures++);
        } else if (type == TextureType::SPECULAR) {
            num = std::to_string(numOfSpecularTextures++);
        }
        else {
            Console::get().error("[OpenGLMesh::draw] Unknown texture type '" + std::to_string(type) + "'");
        }

        texture->bind();
        texture->setTextureUnit(shader, (texture->getTextureTypeAsString() + num).c_str());
    }
}