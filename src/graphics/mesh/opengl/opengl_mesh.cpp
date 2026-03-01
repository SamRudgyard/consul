#include "opengl_mesh.hpp"

#include <memory>
#include <unordered_map>

#include "core/console/console.hpp"
#include "graphics/camera/camera.hpp"
#include "graphics/shaders/shader.hpp"
#include "graphics/textures/opengl/opengl_texture.hpp"
#include "glad/glad.h"
#include "glm/gtc/matrix_inverse.hpp"
#include "utils.hpp"

std::vector<unsigned int> buildLineIndices(const std::vector<unsigned int>& triangleIndices)
{
    if (triangleIndices.size() % 3 != 0) {
        return triangleIndices;
    }

    std::vector<unsigned int> lineIndices;
    lineIndices.reserve((triangleIndices.size() / 3) * 6);

    for (size_t i = 0; i < triangleIndices.size(); i += 3) {
        const unsigned int i0 = triangleIndices[i];
        const unsigned int i1 = triangleIndices[i + 1];
        const unsigned int i2 = triangleIndices[i + 2];

        lineIndices.push_back(i0);
        lineIndices.push_back(i1);
        lineIndices.push_back(i1);
        lineIndices.push_back(i2);
        lineIndices.push_back(i2);
        lineIndices.push_back(i0);
    }

    return lineIndices;
}

std::shared_ptr<OpenGLTexture> OpenGLMesh::getCachedTexture(const Texture& texture, const unsigned int unit)
{
    const std::string& path = texture.getPath();
    auto it = textureCache.find(path);
    if (it != textureCache.end()) {
        if (std::shared_ptr<OpenGLTexture> prevCachedTexture = it->second) {
            return prevCachedTexture;
        }
    }

    std::shared_ptr<OpenGLTexture> newlyCreatedTexture = std::make_shared<OpenGLTexture>(texture, unit);
    textureCache[path] = newlyCreatedTexture;
    return newlyCreatedTexture;
}

OpenGLMesh::OpenGLMesh(Mesh& mesh)
    : RenderableMesh(mesh)
{
    glCheckError();

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glCheckError();

    if (!this->mesh.hasAttribute(AttributeType::POSITION)) {
        Console::get().error("[OpenGLMesh::OpenGLMesh] Provided Mesh has no position data");
        return;
    }

    unsigned int positionVBO = enableVertexBuffer(this->mesh.getPositions(), AttributeType::POSITION, false);
    this->mesh.setVertexBuffer(positionVBO, AttributeType::POSITION);

    glCheckError();

    if (this->mesh.hasAttribute(AttributeType::NORMAL)) {
        unsigned int normalVBO = enableVertexBuffer(this->mesh.getNormals(), AttributeType::NORMAL, false);
        this->mesh.setVertexBuffer(normalVBO, AttributeType::NORMAL);
    } else {
        glVertexAttrib3fv((unsigned int)(AttributeType::NORMAL), glm::value_ptr(glm::vec3(0.0f, 0.0f, 1.0f)));
    }

    glCheckError();
    
    if (this->mesh.hasAttribute(AttributeType::TEXCOORD)) {
        unsigned int texCoordVBO = enableVertexBuffer(this->mesh.getTextureCoords(), AttributeType::TEXCOORD, false);
        this->mesh.setVertexBuffer(texCoordVBO, AttributeType::TEXCOORD);
    } else {
        glVertexAttrib2fv((unsigned int)(AttributeType::TEXCOORD), glm::value_ptr(glm::vec2(0.0f, 0.0f)));
    }

    glCheckError();

    if (this->mesh.hasAttribute(AttributeType::TANGENT)) {
        unsigned int tangentVBO = enableVertexBuffer(this->mesh.getTangents(), AttributeType::TANGENT, false);
        this->mesh.setVertexBuffer(tangentVBO, AttributeType::TANGENT);
    } else {
        glVertexAttrib4fv((unsigned int)(AttributeType::TANGENT), glm::value_ptr(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)));
    }

    glCheckError();

    // Generate element buffer object (EBO) for indices
    std::vector<unsigned int> indices = this->mesh.getIndices();
    if (this->mesh.getDrawMode() == DrawMode::LINES) {
        indices = buildLineIndices(indices);
    }

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    uploadedIndexCount = static_cast<unsigned int>(indices.size());
    this->mesh.setVertexBuffer(ebo, AttributeType::INDICES);
    // Unbind all to prevent accidental modification
    glBindVertexArray(0);                       // Unbind VAO first
    glBindBuffer(GL_ARRAY_BUFFER, 0);           // Then unbind VBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);   // Finally unbind EBO

    glCheckError();

    std::vector<Texture> meshTextures = this->mesh.getTextures();
    unsigned int numOfDiffuseTextures = 0;
    unsigned int numOfSpecularTextures = 0;
    unsigned int numOfNormalTextures = 0;
    unsigned int textureUnit = 0;
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
            case TextureType::NORMAL:
                nTextureType = std::to_string(numOfNormalTextures++);
                break;
            default:
                Console::get().error("[OpenGLMesh::OpenGLMesh] Unsupported texture type");
                continue;
        }
        std::shared_ptr<OpenGLTexture> openglTexture = getCachedTexture(texture, textureUnit);
        textures.insert({texture.getTextureTypeAsString() + nTextureType, openglTexture});
        textureUnit++;
    }

    // Now that the mesh is uploaded to the GPU, we can drop the CPU-side vertex data
    this->mesh.clear();
}

OpenGLMesh::~OpenGLMesh()
{
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &ebo);
    
    unsigned int positionVBO = this->mesh.getVertexBuffer(AttributeType::POSITION);
    glDeleteBuffers(1, &positionVBO);
    
    unsigned int normalVBO = this->mesh.getVertexBuffer(AttributeType::NORMAL);
    if (normalVBO != 0) {
        glDeleteBuffers(1, &normalVBO);
    }

    unsigned int texCoordVBO = this->mesh.getVertexBuffer(AttributeType::TEXCOORD);
    if (texCoordVBO != 0) {
        glDeleteBuffers(1, &texCoordVBO);
    }

    unsigned int tangentVBO = this->mesh.getVertexBuffer(AttributeType::TANGENT);
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

    const Colour tint = this->mesh.getTint();
    shader->setUniformVec4("meshTint", (float)tint.r/255.0f, (float)tint.g/255.0f, (float)tint.b/255.0f, (float)tint.alpha/255.0f);
    glCheckError();

    shader->setUniformInt("useLighting", this->mesh.getDrawMode() == DrawMode::LINES ? 0 : 1);
    glCheckError();

    for (const auto& [name, texture] : textures) {
        texture->bind();
        texture->setTextureUnit(shader, name.c_str());
        glCheckError();
    }

    glCheckError();
    shader->use();

    shader->setUniformVec3("lightPosition", 0.0f, 0.0f, 20.0f);
    glCheckError();
    shader->setUniformVec3("lightColour", 1.0f, 1.0f, 1.0f);
    glCheckError();
    shader->setUniformVec3("ambientColour", 0.15f, 0.15f, 0.15f);
    glCheckError();
    shader->setUniformMat4("model", getModelMatrix());
    glCheckError();
    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(getModelMatrix())));
    shader->setUniformMat3("normalMatrix", normalMatrix);
    glCheckError();

    camera.sendToShader(shader);
    glCheckError();

    int glDrawMode;
    switch (this->mesh.getDrawMode())
    {
    case DrawMode::TRIANGLES:
        glDrawMode = GL_TRIANGLES;
        break;
    case DrawMode::LINES:
        glDrawMode = GL_LINES;
        break;
    
    default:
        Console::get().warn("[OpenGLMesh::draw] Unexpected draw mode");
        return;
    }

    glDrawElements(glDrawMode, uploadedIndexCount, GL_UNSIGNED_INT, 0);
    glCheckError();
}
