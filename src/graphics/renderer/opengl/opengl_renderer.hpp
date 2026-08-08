#pragma once

#include <unordered_map>
#include <vector>

#include "graphics/renderer/renderer.hpp"
#include "glm/fwd.hpp"
#include "glad/glad.h"

enum class AttributeType;

struct ShaderBuffer
{
    GLuint id = 0;
};

struct TextureBuffer
{
    GLuint id = 0;
};

struct MeshBuffer
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
     * @param shaderID Asset ID of the shader to upload to the GPU.
     * @param vertexShader Vertex shader source stage.
     * @param fragmentShader Fragment shader source stage.
     */
    void uploadShader(AssetID shaderID, const VertexShader& vertexShader, const FragmentShader& fragmentShader) override;

    /**
     * Uploads the given Mesh to the GPU.
     * @param meshID Asset ID of the mesh to upload to the GPU.
     * @param mesh The mesh data to upload to the GPU.
     */
    void uploadMesh(AssetID meshID, Mesh& mesh) override;

    /**
     * Uploads the given Texture to the GPU.
     * @param textureID Asset ID of the texture to upload to the GPU.
     * @param texture The texture data to upload to the GPU.
     */
    void uploadTexture(AssetID textureID, Texture& texture) override;

    /**
     * Render all uploaded models/meshes with the provided shader and camera.
     * @param shaderID Asset ID of the shader to render with.
     * @param camera The camera, from which the models/meshes are viewed. 
     */
    void render(AssetID shaderID, const Camera& camera, AssetLibrary& assets) override;

private:
    std::unordered_map<AssetID, ShaderBuffer> shaders;
    std::unordered_map<AssetID, MeshBuffer> meshes;
    std::unordered_map<AssetID, TextureBuffer> textures;

    unsigned int enableVertexBuffer(const std::vector<glm::vec2>& data, AttributeType attribute, bool useDynamicDraw);
    unsigned int enableVertexBuffer(const std::vector<glm::vec3>& data, AttributeType attribute, bool useDynamicDraw);
    unsigned int enableVertexBuffer(const std::vector<glm::vec4>& data, AttributeType attribute, bool useDynamicDraw);
    void bindTexture(GLuint programID, GLuint textureUnit, const char* uniformName, AssetID textureID);
    static void setUniformInt(GLuint programID, const char* uniformName, int value);
    static void setUniformFloat(GLuint programID, const char* uniformName, float value);
    static void setUniformVec2(GLuint programID, const char* uniformName, const glm::vec2& value);
    static void setUniformVec3(GLuint programID, const char* uniformName, const glm::vec3& value);
    static void setUniformVec4(GLuint programID, const char* uniformName, const glm::vec4& value);
    static void setUniformMat3(GLuint programID, const char* uniformName, const glm::mat3& value);
    static void setUniformMat4(GLuint programID, const char* uniformName, const glm::mat4& value);
    void releaseMesh(MeshBuffer& mesh);
    void releaseShader(ShaderBuffer& shader);
    void releaseTexture(TextureBuffer& texture);
};
