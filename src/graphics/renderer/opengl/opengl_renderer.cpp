#include "opengl_renderer.hpp"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif
#include <stb_image.h>

OpenGLRenderer::~OpenGLRenderer()
{
    for (auto& [shaderID, ShaderBuffer] : shaders) {
        releaseShader(ShaderBuffer);
    }

    for (auto& [meshID, meshBuffer] : meshes) {
        releaseMesh(meshBuffer);
    }

    for (auto& [textureID, textureBuffer] : textures) {
        releaseTexture(textureBuffer);
    }

    shaders.clear();
    meshes.clear();
    textures.clear();
}

void OpenGLRenderer::initialiseGraphics(void* loaderFunc)
{
    gladLoadGLLoader((GLADloadproc)loaderFunc);

    const GLubyte* version = glGetString(GL_VERSION);
    if (!version) {
        Console::get().error("[OpenGL] Failed to retrieve OpenGL version");
    } else {
        Console::get().log("[OpenGL] Found OpenGL version " + std::string(reinterpret_cast<const char*>(version)));
    }

    glCheckError();

    glEnable(GL_DEPTH_TEST);                                // Enable depth testing
    glDepthFunc(GL_LESS);                                   // Type of depth testing to apply
    glClearDepth(1.0f);                                     // Clear depth buffer to farthest
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);      // Colour blending, determines how pixel colours are combined
    glEnable(GL_BLEND);                                     // Enable colour blending (required for transparencies)
    glCullFace(GL_BACK);                                    // Cull back faces
    glFrontFace(GL_CCW);                                    // Front faces are counter clockwise
    glEnable(GL_CULL_FACE);                                 // Enable backface culling
}

void OpenGLRenderer::initialiseImGui()
{
    ImGui_ImplOpenGL3_Init("#version 330");
}

void OpenGLRenderer::clearBackground(const glm::vec4& colour)
{
    glClearColor(colour.r, colour.g, colour.b, colour.a);
    clearScreenBuffer();
}

void OpenGLRenderer::clearScreenBuffer()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLRenderer::clearSceneResources()
{
    for (auto& [meshID, meshBuffer] : meshes) {
        releaseMesh(meshBuffer);
    }
    meshes.clear();
    meshDrawOrder.clear();

    for (auto& [shaderID, shaderBuffer] : shaders) {
        releaseShader(shaderBuffer);
    }
    shaders.clear();

    for (auto& [textureID, textureBuffer] : textures) {
        releaseTexture(textureBuffer);
    }
    textures.clear();
}

void OpenGLRenderer::setViewport(int x, int y, int width, int height)
{
    if (width < 0) {
        Console::get().error("[OpenGLRenderer::setViewport] Viewport width cannot be -ve: " + std::to_string(width));
        return;
    }
    if (height < 0) {
        Console::get().error("[OpenGLRenderer::setViewport] Viewport height cannot be -ve: " + std::to_string(height));
        return;
    }

    glViewport(x, y, width, height);
}

void OpenGLRenderer::uploadShader(Shader& shader)
{
    const unsigned int shaderID = shader.getID();
    if (shaders.find(shaderID) != shaders.end()) {
        // Currently no functionality to adjust Shader vertex/fragment code
        // on the fly, so return if found.
        return;
    }

    unsigned int vertexID, fragmentID;
    vertexID = glCreateShader(GL_VERTEX_SHADER);
    fragmentID = glCreateShader(GL_FRAGMENT_SHADER);
    glCheckError();

    // Compile vertex shader
    const char* vertexCString = shader.getVertexSource().c_str();
    glShaderSource(vertexID, 1, &vertexCString, NULL);
    glCompileShader(vertexID);
    glCheckError();

    // Compile fragment shader
    const char* fragmentCString = shader.getFragmentSource().c_str();
    glShaderSource(fragmentID, 1, &fragmentCString, NULL);
    glCompileShader(fragmentID);
    glCheckError();

    // Link shaders into a program
    GLuint programID = glCreateProgram();
    glAttachShader(programID, vertexID);
    glAttachShader(programID, fragmentID);
    glLinkProgram(programID);
    glCheckError();

    // Clean up shaders
    glDeleteShader(vertexID);
    glDeleteShader(fragmentID);
    glCheckError();

    // Store the compiled shader directly in the map entry.
    shaders[shaderID].id = programID;

    Console::get().logOnDebug("[OpenGLRenderer::uploadShader] Successfully uploaded Shader " + std::to_string(shader.getID()) + " to GPU.");
}

void OpenGLRenderer::uploadMesh(Mesh& mesh)
{
    if (!mesh.isAnyDirty()) return;

    auto [it, inserted] = meshes.try_emplace(mesh.getID());
    MeshBuffer& meshBuffer = it->second;
    meshBuffer.mesh = &mesh;

    if (inserted) {
        glGenVertexArrays(1, &meshBuffer.vao);
    }
    glBindVertexArray(meshBuffer.vao);
    glCheckError();

    const bool uploadPositions = inserted || mesh.isDirty(AttributeType::POSITION);
    const bool uploadNormals = inserted || mesh.isDirty(AttributeType::NORMAL);
    const bool uploadTexCoords = inserted || mesh.isDirty(AttributeType::TEXCOORD);
    const bool uploadTangents = inserted || mesh.isDirty(AttributeType::TANGENT);
    const bool uploadIndices = inserted || mesh.isDirty(AttributeType::INDICES);

    if (uploadPositions) {
        if (!mesh.hasAttribute(AttributeType::POSITION)) {
            Console::get().error("[OpenGLRenderer::uploadMesh] Provided Mesh has no position data");
            return;
        }
        meshBuffer.positionVBO = enableVertexBuffer(mesh.getPositions(), AttributeType::POSITION, false);
        mesh.clean(AttributeType::POSITION);
        glCheckError();
    }
    
    if (uploadNormals) {
        if (mesh.hasAttribute(AttributeType::NORMAL)) {
            meshBuffer.normalVBO = enableVertexBuffer(mesh.getNormals(), AttributeType::NORMAL, false);
        } else {
            glVertexAttrib3fv((unsigned int)(AttributeType::NORMAL), glm::value_ptr(glm::vec3(0.0f, 0.0f, 1.0f)));
        }
        mesh.clean(AttributeType::NORMAL);
        glCheckError();
    }

    if (uploadTexCoords) {
        if (mesh.hasAttribute(AttributeType::TEXCOORD)) {
            meshBuffer.texCoordVBO = enableVertexBuffer(mesh.getTextureCoords(), AttributeType::TEXCOORD, false);
        } else {
            glVertexAttrib2fv((unsigned int)(AttributeType::TEXCOORD), glm::value_ptr(glm::vec2(0.0f, 0.0f)));
        }
        mesh.clean(AttributeType::TEXCOORD);
        glCheckError();
    }

    if (uploadTangents) {
        if (mesh.hasAttribute(AttributeType::TANGENT)) {
            meshBuffer.tangentVBO = enableVertexBuffer(mesh.getTangents(), AttributeType::TANGENT, false);
        } else {
            glVertexAttrib4fv((unsigned int)(AttributeType::TANGENT), glm::value_ptr(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)));
        }
        mesh.clean(AttributeType::TANGENT);
        glCheckError();
    }    

    if (uploadIndices) {
        // Generate element buffer object (EBO) for indices
        std::vector<unsigned int> indices = mesh.getIndices();

        GLuint ebo = 0;
        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
        glCheckError();
        meshBuffer.ebo = ebo;
        mesh.clean(AttributeType::INDICES);
    }

    // Unbind all to prevent accidental modification
    glBindVertexArray(0);                       // Unbind VAO first
    glBindBuffer(GL_ARRAY_BUFFER, 0);           // Then unbind VBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);   // Finally unbind EBO
    glCheckError();

    for (Texture& texture : mesh.getTextures()) {
        uploadTexture(texture);
    }

    Console::get().logOnDebug("[OpenGLRenderer::uploadMesh] Successfully uploaded Mesh " + std::to_string(mesh.getID()) + " to GPU.");
}

void OpenGLRenderer::uploadModel(Model& model)
{
    std::vector<Mesh>& modelMeshes = model.getMeshes();
    for (Mesh& mesh : modelMeshes) {
        uploadMesh(mesh);
    }

    Console::get().logOnDebug("[OpenGLRenderer::uploadModel] Successfully uploaded Model '" + model.getFilePath() + "' to GPU.");
}

void OpenGLRenderer::uploadTexture(Texture& texture)
{
    const unsigned int textureID = texture.getID();
    const std::string& texturePath = texture.getPath();

    if (textures.find(textureID) != textures.end()) {
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

    glCheckError();

    TextureBuffer& textureBuffer = textures[textureID];
    glGenTextures(1, &textureBuffer.id);
    glBindTexture(GL_TEXTURE_2D, textureBuffer.id);
    glCheckError();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glCheckError();

    if (numColourChannels == 4) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    } else if (numColourChannels == 3) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    } else if (numColourChannels == 1) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, textureWidth, textureHeight, 0, GL_RED, GL_UNSIGNED_BYTE, data);
    } else {
        stbi_image_free(data);
        glBindTexture(GL_TEXTURE_2D, 0);
        releaseTexture(textureBuffer);
        textures.erase(textureID);
        Console::get().error("[OpenGLRenderer::uploadTexture] Invalid number of colour channels (expected 1, 3, or 4, but got " + std::to_string(numColourChannels) + ")");
        return;
    }
    glCheckError();

    Console::get().logOnDebug("[OpenGLRenderer::uploadTexture] Successfully uploaded Texture " + std::to_string(texture.getID()) + " to GPU.");

    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
    glCheckError();
}

void OpenGLRenderer::render(const Shader& shader, const Camera& camera)
{
    const auto shaderIt = shaders.find(shader.getID());
    if (shaderIt == shaders.end()) {
        Console::get().error("[OpenGLRenderer::render] Attempting to render with a shader that hasn't been uploaded.");
        return;
    }
    const GLuint programID = shaderIt->second.id;
    glUseProgram(programID);
    glCheckError();

    setUniformMat4(programID, "cameraMatrix", camera.getCameraMatrix());
    setUniformVec3(programID, "cameraPosition", camera.getPosition());
    setUniformVec3(programID, "lightPosition", glm::vec3(5.0f, 5.0f, 5.0f));
    setUniformVec3(programID, "lightColour", glm::vec3(1.0f, 1.0f, 1.0f));
    setUniformVec3(programID, "ambientColour", glm::vec3(0.2f, 0.2f, 0.2f));

    for (auto& [meshID, meshBuffer] : meshes) {
        if (!meshBuffer.mesh) {
            continue;
        }
        const Mesh& mesh = *meshBuffer.mesh;
        const std::vector<Texture>& textures = mesh.getTextures();

        unsigned int iDiffuse = 0;
        unsigned int iSpecular = 0;

        for (unsigned int iTexture = 0; iTexture < textures.size(); iTexture++) {
            const Texture& texture = textures[iTexture];
            if (texture.getType() == TextureType::DIFFUSE) {
                const std::string uniformName = "diffuse" + std::to_string(iDiffuse);
                glCheckError();
                bindTexture(programID, 0, uniformName.c_str(), texture);
                iDiffuse++;
            }
            else if (texture.getType() == TextureType::SPECULAR) {
                const std::string uniformName = "specular" + std::to_string(iSpecular);
                bindTexture(programID, 1, uniformName.c_str(), texture);
                glCheckError();
                iSpecular++;
            }
        }

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

        glBindVertexArray(meshBuffer.vao);
        glDrawElements(
            mesh.getDrawMode() == DrawMode::LINES ? GL_LINES : GL_TRIANGLES,
            static_cast<GLsizei>(mesh.getNumIndices()),
            GL_UNSIGNED_INT,
            nullptr
        );

        glCheckError();
    }

    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
    glUseProgram(0);
    glCheckError();
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

void OpenGLRenderer::bindTexture(GLuint programID, GLuint textureUnit, const char* uniformName, const Texture& texture)
{
    auto it = textures.find(texture.getID());
    if (it == textures.end()) {
        Console::get().error("[OpenGLRenderer::bindTexture] Attempting to bind texture that hasn't been uploaded!");
        return;
    }

    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, it->second.id);
    setUniformInt(programID, uniformName, (int)textureUnit);
}

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
        glCheckError();
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
        glCheckError();
    }
}


void OpenGLRenderer::releaseMesh(MeshBuffer& mesh)
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

void OpenGLRenderer::releaseShader(ShaderBuffer& shader)
{
    if (shader.id != 0) {
        glDeleteProgram(shader.id);
        shader.id = 0;
    }
}

void OpenGLRenderer::releaseTexture(TextureBuffer& texture)
{
    if (texture.id != 0) {
        glDeleteTextures(1, &texture.id);
        texture.id = 0;
    }
}
