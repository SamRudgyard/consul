#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "core/console/console.hpp"
#include "graphics/camera/camera.hpp"
#include "graphics/mesh.hpp"
#include "graphics/models/model.hpp"
#include "graphics/shader.hpp"
#include "graphics/texture.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glad/glad.h"
#include "imgui_impl_opengl3.h"
#include "renderer.hpp"
#include "utils.hpp"

struct OpenGLShader
{
    GLuint id = 0;
};

struct OpenGLTexture
{
    GLuint id = 0;
};

struct OpenGLMesh
{
    GLuint vao = 0;
    GLuint positionVBO = 0;
    GLuint normalVBO = 0;
    GLuint texCoordVBO = 0;
    GLuint tangentVBO = 0;
    GLuint ebo = 0;
};

class OpenGLRenderer : public Renderer
{
public:
    OpenGLRenderer() = default;
    ~OpenGLRenderer() override;

    void initialiseGraphics(void* loaderFunc) override
    {
        gladLoadGLLoader((GLADloadproc) loaderFunc);

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

    void initialiseImGui() override
    {
        ImGui_ImplOpenGL3_Init("#version 330");
    }

    void clearBackground(const glm::vec4& colour) override
    {
        glClearColor(colour.r, colour.g, colour.b, colour.a);
        clearScreenBuffer();
    }

    void clearScreenBuffer() override
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void setViewport(int x, int y, int width, int height) override
    {
        glViewport(x, y, width, height);
    }

    void uploadShader(const Shader& shader) override
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

    void uploadMesh(const Mesh& mesh) override
    {
        unsigned int id = mesh.getID();
        meshData.insert_or_assign(id, mesh);

        if (meshes.find(id) != meshes.end()) {
            return; // Mesh data already tracked, GPU geometry already uploaded
        }

        OpenGLMesh& glMesh = meshes[id];
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

    void uploadTexture(const Texture& texture) override;

    void uploadModel(Model& model) override
    {
        const std::vector<Mesh>& modelMeshes = model.getMeshes();
        for (const Mesh& mesh : modelMeshes) {
            uploadMesh(mesh);
        }
    }

    void clearSceneResources() override;

    void render(const Shader& shader, const Camera& camera) override;

private:
    std::unordered_map<unsigned int, OpenGLShader> shaders;
    std::unordered_map<unsigned int, OpenGLMesh> meshes;
    std::unordered_map<unsigned int, Mesh> meshData;
    std::vector<unsigned int> meshDrawOrder;
    std::unordered_map<std::string, OpenGLTexture> textures;

    unsigned int enableVertexBuffer(const std::vector<glm::vec2>& data, AttributeType attribute, bool useDynamicDraw);
    unsigned int enableVertexBuffer(const std::vector<glm::vec3>& data, AttributeType attribute, bool useDynamicDraw);
    unsigned int enableVertexBuffer(const std::vector<glm::vec4>& data, AttributeType attribute, bool useDynamicDraw);
    void bindTexture(GLuint programID, GLuint textureUnit, const char* uniformName, const Texture& texture);
    static void setUniformInt(GLuint programID, const char* uniformName, int value);
    static void setUniformVec3(GLuint programID, const char* uniformName, const glm::vec3& value);
    static void setUniformVec4(GLuint programID, const char* uniformName, const glm::vec4& value);
    static void setUniformMat3(GLuint programID, const char* uniformName, const glm::mat3& value);
    static void setUniformMat4(GLuint programID, const char* uniformName, const glm::mat4& value);
    void releaseMesh(OpenGLMesh& mesh);
    void releaseShader(OpenGLShader& shader);
    void releaseTexture(OpenGLTexture& texture);
};
