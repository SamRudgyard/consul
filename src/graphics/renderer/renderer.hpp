#pragma once

#include "glm/glm.hpp"

#include <memory>
#include <vector>

enum class GraphicsAPI
{
    OpenGL,
    // Future graphics APIs can be added here (e.g., Vulkan, DirectX, etc.)
};

class Material;
class Mesh;
class Shader;
class Texture;
class Scene;

struct RenderItem
{
    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<Material> material;
    glm::mat4 modelMatrix = glm::mat4(1.0f);
};

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

    virtual void releaseExpiredResources() = 0;

    virtual void render(const Scene& scene) = 0;

protected:
    virtual void uploadShader(const std::shared_ptr<Shader>& shader) = 0;

    virtual void uploadMesh(const std::shared_ptr<Mesh>& mesh) = 0;

    virtual void uploadTexture(const std::shared_ptr<Texture>& texture) = 0;
};
