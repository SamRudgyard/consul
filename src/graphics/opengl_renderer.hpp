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

    /**
     * Initialises GLAD, allowing us to call OpenGL functions.
     * @param loaderFunc Function to load the OS-specific
     * OpenGL function pointers. 
     */
    void initialiseGraphics(void* loaderFunc) override;

    /**
     * Initialise ImGui for OpenGL version 3.3.0.
     */
    void initialiseImGui() override;

    /**
     * Clears the background to the given colour.
     * @param colour Colour to set the background to on clear.
     */
    void clearBackground(const glm::vec4& colour) override;

    /**
     * Clears the screen buffer.
     */
    void clearScreenBuffer() override;

    /**
     * Releases all resources previously uploaded to the GPU. 
     */
    void clearSceneResources() override;

    /**
     * Sets the viewport (rectangle in which we render). Note that
     * the given vector `(x, y)` specifies the lower left corner of
     * the viewport rectangle.
     * @param x Lower left x-coordinate of the viewport rectangle, in pixels.
     * @param y Lower left y-coordinate of the viewport rectangle, in pixels.
     * @param width The width of the viewport, in pixels. 
     * @param height The height of the viewport, in pixels.
     */
    void setViewport(int x, int y, int width, int height) override;

    /**
     * Uploads the given Shader to the GPU.
     * @param shader The shader to upload to the GPU.
     */
    void uploadShader(const Shader& shader) override;

    /**
     * Uploads the given Mesh to the GPU.
     * @param mesh The mesh to upload to the GPU.
     */
    void uploadMesh(const Mesh& mesh) override;

    /**
     * Uploads the given Model to the GPU.
     * @param model The model to upload to the GPU.
     */
    void uploadModel(Model& model) override;

    /**
     * Uploads the given Texture to the GPU.
     * @param texture The texture to upload to the GPU.
     */
    void uploadTexture(const Texture& texture) override;

    /**
     * Render all uploaded models/meshes with the provided shader and camera.
     * @param shader The shader to render with.
     * @param camera The camera, from which the models/meshes are viewed. 
     */
    void render(const Shader& shader, const Camera& camera) override;

private:
    std::unordered_map<unsigned int, OpenGLShader> shaders;
    std::unordered_map<unsigned int, OpenGLMesh> openglMeshes;
    std::unordered_map<unsigned int, Mesh> meshes;
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
