#pragma once

#include "glm/fwd.hpp"

enum class GraphicsAPI
{
    OpenGL,
    // Future graphics APIs can be added here (e.g., Vulkan, DirectX, etc.)
};

class Camera;
class Mesh;
class Model;
class Shader;
class Texture;

class Renderer
{
public:
    Renderer() = default;
    virtual ~Renderer() = default;

    virtual void initialiseGraphics(void* loaderFunc) = 0;

    virtual void initialiseImGui() = 0;

    virtual void clearBackground(const glm::vec4& colour) = 0;

    virtual void clearScreenBuffer() = 0;

    virtual void setViewport(int x, int y, int width, int height) = 0;

    virtual void uploadShader(const Shader& shader) = 0;

    virtual void uploadMesh(const Mesh& mesh) = 0;

    virtual void uploadTexture(const Texture& texture) = 0;

    virtual void uploadModel(Model& model) = 0;

    virtual void clearSceneResources() = 0;

    virtual void render(const Shader& shader, const Camera& camera) = 0;
};
