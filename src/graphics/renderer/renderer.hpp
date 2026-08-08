#pragma once

#include "glm/fwd.hpp"

#include <memory>

enum class GraphicsAPI
{
    OpenGL,
    // Future graphics APIs can be added here (e.g., Vulkan, DirectX, etc.)
};

class Camera;
class AssetLibrary;
class Mesh;
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

    virtual void uploadShader(const std::shared_ptr<Shader>& shader) = 0;

    virtual void uploadMesh(const std::shared_ptr<Mesh>& mesh) = 0;

    virtual void uploadTexture(const std::shared_ptr<Texture>& texture) = 0;

    virtual void releaseExpiredResources() = 0;

    virtual void render(const std::shared_ptr<Shader>& shader, const Camera& camera, AssetLibrary& assets) = 0;
};
