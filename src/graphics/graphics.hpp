#pragma once

#include "glm/vec4.hpp"
#include "shaders/shader.hpp"
#include "graphics/mesh/renderable_mesh.hpp"
#include "graphics/mesh/mesh_data.hpp"

enum class GraphicsAPI
{
    OpenGL,
    // Future graphics APIs can be added here (e.g., Vulkan, DirectX, etc.)
};

class IGraphics
{
public:
    virtual void loadGraphics(void* loaderFunc) = 0;
    virtual void clearColour(const glm::vec4& colour) = 0;

    virtual IShader* newShader() = 0;
    virtual RenderableMesh* newMesh(MeshData& meshData) = 0;
};