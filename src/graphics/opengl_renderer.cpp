#include "opengl_renderer.hpp"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif
#include <stb_image.h>

OpenGLRenderer::~OpenGLRenderer()
{
    for (auto& [id, shader] : shaders) {
        releaseShader(shader);
    }

    for (auto& [id, mesh] : meshes) {
        releaseMesh(mesh);
    }

    for (auto& [path, texture] : textures) {
        releaseTexture(texture);
    }
}

unsigned int OpenGLRenderer::enableVertexBuffer(const std::vector<glm::vec2>& data, AttributeType attribute, bool useDynamicDraw)
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
void OpenGLRenderer::releaseMesh(OpenGLMesh& mesh)
{
    if (mesh.positionVBO != 0) {
        glDeleteBuffers(1, &mesh.positionVBO);
        mesh.positionVBO = 0;
    }

    if (mesh.normalVBO != 0) {
        glDeleteBuffers(1, &mesh.normalVBO);
        mesh.normalVBO = 0;
    }

    if (mesh.texCoordVBO != 0) {
        glDeleteBuffers(1, &mesh.texCoordVBO);
        mesh.texCoordVBO = 0;
    }

    if (mesh.tangentVBO != 0) {
        glDeleteBuffers(1, &mesh.tangentVBO);
        mesh.tangentVBO = 0;
    }

    if (mesh.ebo != 0) {
        glDeleteBuffers(1, &mesh.ebo);
        mesh.ebo = 0;
    }

    if (mesh.vao != 0) {
        glDeleteVertexArrays(1, &mesh.vao);
        mesh.vao = 0;
    }
}

void OpenGLRenderer::releaseShader(OpenGLShader& shader)
{
    if (shader.id != 0) {
        glDeleteProgram(shader.id);
        shader.id = 0;
    }
}

void OpenGLRenderer::releaseTexture(OpenGLTexture& texture)
{
    if (texture.id != 0) {
        glDeleteTextures(1, &texture.id);
        texture.id = 0;
    }
}

void OpenGLRenderer::uploadTexture(const Texture& texture)
{
    const std::string& texturePath = texture.getPath();
    if (textures.find(texturePath) != textures.end()) {
        return; // Texture already uploaded, so return
    }

    if (!doesFileExist(texturePath.c_str())) {
        Console::get().error("[OpenGLRenderer::uploadTexture] Texture file does not exist: '" + texturePath + "'");
        return;
    }

    int textureWidth = 0;
    int textureHeight = 0;
    int numColourChannels = 0;

    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(texturePath.c_str(), &textureWidth, &textureHeight, &numColourChannels, 0);
    if (!data) {
        Console::get().error("[OpenGLRenderer::uploadTexture] Texture file could not be loaded: '" + texturePath + "'");
        return;
    }

    OpenGLTexture& glTexture = textures[texturePath];
    glGenTextures(1, &glTexture.id);
    glBindTexture(GL_TEXTURE_2D, glTexture.id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    if (numColourChannels == 4) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    } else if (numColourChannels == 3) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    } else if (numColourChannels == 1) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, textureWidth, textureHeight, 0, GL_RED, GL_UNSIGNED_BYTE, data);
    } else {
        stbi_image_free(data);
        glBindTexture(GL_TEXTURE_2D, 0);
        releaseTexture(glTexture);
        textures.erase(texturePath);
        Console::get().error("[OpenGLRenderer::uploadTexture] Invalid number of colour channels (expected 1, 3, or 4, but got " + std::to_string(numColourChannels) + ")");
        return;
    }

    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
    glCheckError();
}

unsigned int OpenGLRenderer::enableVertexBuffer(const std::vector<glm::vec3>& data, AttributeType attribute, bool useDynamicDraw)
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

unsigned int OpenGLRenderer::enableVertexBuffer(const std::vector<glm::vec4>& data, AttributeType attribute, bool useDynamicDraw)
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
