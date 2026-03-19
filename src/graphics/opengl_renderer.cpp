#include "opengl_renderer.hpp"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif
#include <stb_image.h>

void OpenGLRenderer::setUniformInt(GLuint programID, const char* uniformName, int value)
{
    const GLint location = glGetUniformLocation(programID, uniformName);
    if (location >= 0) {
        glUniform1i(location, value);
    }
}

void OpenGLRenderer::setUniformVec3(GLuint programID, const char* uniformName, const glm::vec3& value)
{
    const GLint location = glGetUniformLocation(programID, uniformName);
    if (location >= 0) {
        glUniform3fv(location, 1, glm::value_ptr(value));
    }
}

void OpenGLRenderer::setUniformVec4(GLuint programID, const char* uniformName, const glm::vec4& value)
{
    const GLint location = glGetUniformLocation(programID, uniformName);
    if (location >= 0) {
        glUniform4fv(location, 1, glm::value_ptr(value));
    }
}

void OpenGLRenderer::setUniformMat3(GLuint programID, const char* uniformName, const glm::mat3& value)
{
    const GLint location = glGetUniformLocation(programID, uniformName);
    if (location >= 0) {
        glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }
}

void OpenGLRenderer::setUniformMat4(GLuint programID, const char* uniformName, const glm::mat4& value)
{
    const GLint location = glGetUniformLocation(programID, uniformName);
    if (location >= 0) {
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }
}

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

void OpenGLRenderer::bindTexture(GLuint programID, GLuint textureUnit, const char* uniformName, const Texture& texture)
{
    uploadTexture(texture);

    const auto it = textures.find(texture.getPath());
    if (it == textures.end()) {
        return;
    }

    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, it->second.id);
    setUniformInt(programID, uniformName, static_cast<int>(textureUnit));
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

void OpenGLRenderer::clearSceneResources()
{
    for (auto& [id, mesh] : meshes) {
        releaseMesh(mesh);
    }
    meshes.clear();
    meshData.clear();
    meshDrawOrder.clear();

    for (auto& [id, shader] : shaders) {
        releaseShader(shader);
    }
    shaders.clear();

    for (auto& [path, texture] : textures) {
        releaseTexture(texture);
    }
    textures.clear();
}

void OpenGLRenderer::render(const Shader& shader, const Camera& camera)
{
    if (meshDrawOrder.empty()) {
        return;
    }

    uploadShader(shader);

    const auto shaderIt = shaders.find(shader.getID());
    if (shaderIt == shaders.end()) {
        Console::get().error("[OpenGLRenderer::render] Attempted to render with a shader that has not been uploaded");
        return;
    }

    const GLuint programID = shaderIt->second.id;
    glUseProgram(programID);

    setUniformMat4(programID, "cameraMatrix", camera.getCameraMatrix());
    setUniformVec3(programID, "cameraPosition", camera.getPosition());
    setUniformVec3(programID, "lightPosition", glm::vec3(5.0f, 5.0f, 5.0f));
    setUniformVec3(programID, "lightColour", glm::vec3(1.0f, 1.0f, 1.0f));
    setUniformVec3(programID, "ambientColour", glm::vec3(0.2f, 0.2f, 0.2f));

    const Texture defaultDiffuseTexture = Texture("assets/default/default.png", TextureType::DIFFUSE);
    const Texture defaultSpecularTexture = Texture("assets/default/default.png", TextureType::SPECULAR);

    for (unsigned int meshID : meshDrawOrder) {
        const auto meshIt = meshData.find(meshID);
        const auto glMeshIt = meshes.find(meshID);
        if (meshIt == meshData.end() || glMeshIt == meshes.end()) {
            continue;
        }

        const Mesh& mesh = meshIt->second;
        const OpenGLMesh& glMesh = glMeshIt->second;

        Texture diffuseTexture = defaultDiffuseTexture;
        Texture specularTexture = defaultSpecularTexture;
        for (const Texture& texture : mesh.getTextures()) {
            if (texture.getType() == TextureType::DIFFUSE && diffuseTexture == defaultDiffuseTexture) {
                diffuseTexture = texture;
            } else if (texture.getType() == TextureType::SPECULAR && specularTexture == defaultSpecularTexture) {
                specularTexture = texture;
            }
        }

        bindTexture(programID, 0, "diffuse0", diffuseTexture);
        bindTexture(programID, 1, "specular0", specularTexture);

        const glm::mat4& modelMatrix = mesh.getModelMatrix();
        const glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));
        const Colour tint = mesh.getTint();
        const glm::vec4 tintValue(
            tint.r / 255.0f,
            tint.g / 255.0f,
            tint.b / 255.0f,
            tint.alpha / 255.0f
        );

        setUniformMat4(programID, "model", modelMatrix);
        setUniformMat3(programID, "normalMatrix", normalMatrix);
        setUniformVec4(programID, "meshTint", tintValue);
        setUniformInt(programID, "useLighting", mesh.hasAttribute(AttributeType::NORMAL) ? 1 : 0);

        glBindVertexArray(glMesh.vao);
        glDrawElements(
            mesh.getDrawMode() == DrawMode::LINES ? GL_LINES : GL_TRIANGLES,
            static_cast<GLsizei>(mesh.getNumIndices()),
            GL_UNSIGNED_INT,
            nullptr
        );
    }

    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
    glUseProgram(0);
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
