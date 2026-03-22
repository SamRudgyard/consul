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

    for (auto& [id, mesh] : openglMeshes) {
        releaseMesh(mesh);
    }

    for (auto& [path, texture] : textures) {
        releaseTexture(texture);
    }
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
    for (auto& [id, mesh] : openglMeshes) {
        releaseMesh(mesh);
    }
    openglMeshes.clear();
    meshes.clear();
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

void OpenGLRenderer::uploadShader(const Shader& shader)
{
    if (shaders.find(shader.getID()) != shaders.end()) {
        return; // Shader already uploaded
    }

    unsigned int vertexID, fragmentID;
    vertexID = glCreateShader(GL_VERTEX_SHADER);
    fragmentID = glCreateShader(GL_FRAGMENT_SHADER);

    // Compile vertex shader
    const char* vertexCString = shader.getVertexSource().c_str();
    glShaderSource(vertexID, 1, &vertexCString, NULL);
    glCompileShader(vertexID);

    // Compile fragment shader
    const char* fragmentCString = shader.getFragmentSource().c_str();
    glShaderSource(fragmentID, 1, &fragmentCString, NULL);
    glCompileShader(fragmentID);

    // Link shaders into a program
    GLuint programID = glCreateProgram();
    glAttachShader(programID, vertexID);
    glAttachShader(programID, fragmentID);
    glLinkProgram(programID);

    // Clean up shaders
    glDeleteShader(vertexID);
    glDeleteShader(fragmentID);

    // Store the compiled shader
    shaders[shader.getID()] = { programID };
}

void OpenGLRenderer::uploadMesh(const Mesh& mesh)
{
    unsigned int id = mesh.getID();

    meshes.insert_or_assign(id, mesh);

    if (openglMeshes.find(id) != openglMeshes.end()) {
        return; // Mesh data already tracked, GPU geometry already uploaded
    }

    OpenGLMesh& glMesh = openglMeshes[id];
    meshDrawOrder.push_back(id);
    for (const Texture& texture : mesh.getTextures()) {
        uploadTexture(texture);
    }

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glMesh.vao = vao;

    if (!mesh.hasAttribute(AttributeType::POSITION)) {
        Console::get().error("[OpenGLRenderer::uploadMesh] Provided Mesh has no position data");
        return;
    }
    glMesh.positionVBO = enableVertexBuffer(mesh.getPositions(), AttributeType::POSITION, false);

    if (mesh.hasAttribute(AttributeType::NORMAL)) {
        glMesh.normalVBO = enableVertexBuffer(mesh.getNormals(), AttributeType::NORMAL, false);
    } else {
        glVertexAttrib3fv((unsigned int)(AttributeType::NORMAL), glm::value_ptr(glm::vec3(0.0f, 0.0f, 1.0f)));
    }

    if (mesh.hasAttribute(AttributeType::TEXCOORD)) {
        glMesh.texCoordVBO = enableVertexBuffer(mesh.getTextureCoords(), AttributeType::TEXCOORD, false);
    } else {
        glVertexAttrib2fv((unsigned int)(AttributeType::TEXCOORD), glm::value_ptr(glm::vec2(0.0f, 0.0f)));
    }

    if (mesh.hasAttribute(AttributeType::TANGENT)) {
        glMesh.tangentVBO = enableVertexBuffer(mesh.getTangents(), AttributeType::TANGENT, false);
    } else {
        glVertexAttrib4fv((unsigned int)(AttributeType::TANGENT), glm::value_ptr(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)));
    }

    // Generate element buffer object (EBO) for indices
    std::vector<unsigned int> indices = mesh.getIndices();

    GLuint ebo = 0;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    glMesh.ebo = ebo;
    
    // Unbind all to prevent accidental modification
    glBindVertexArray(0);                       // Unbind VAO first
    glBindBuffer(GL_ARRAY_BUFFER, 0);           // Then unbind VBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);   // Finally unbind EBO
}

void OpenGLRenderer::uploadModel(Model& model)
{
    const std::vector<Mesh>& modelMeshes = model.getMeshes();
    for (const Mesh& mesh : modelMeshes) {
        uploadMesh(mesh);
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
        const auto meshIt = meshes.find(meshID);
        const auto glMeshIt = openglMeshes.find(meshID);
        if (meshIt == meshes.end() || glMeshIt == openglMeshes.end()) {
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
    uploadTexture(texture);

    const auto it = textures.find(texture.getPath());
    if (it == textures.end()) {
        return;
    }

    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, it->second.id);
    setUniformInt(programID, uniformName, static_cast<int>(textureUnit));
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
