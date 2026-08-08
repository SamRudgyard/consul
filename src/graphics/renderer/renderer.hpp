#pragma once

#include "core/project/asset_types.hpp"
#include "glm/fwd.hpp"

#include <memory>

enum class GraphicsAPI
{
    OpenGL,
    // Future graphics APIs can be added here (e.g., Vulkan, DirectX, etc.)
};

class Camera;
class AssetLibrary;
class FragmentShader;
class Mesh;
class Texture;
class VertexShader;

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

    virtual void uploadShader(AssetID shaderID, const VertexShader& vertexShader, const FragmentShader& fragmentShader) = 0;

    virtual void uploadMesh(AssetID meshID, Mesh& mesh) = 0;

    virtual void uploadTexture(const std::shared_ptr<Texture>& texture) = 0;

    virtual void clearSceneResources() = 0;

    virtual void render(AssetID shaderID, const Camera& camera, AssetLibrary& assets) = 0;
};
