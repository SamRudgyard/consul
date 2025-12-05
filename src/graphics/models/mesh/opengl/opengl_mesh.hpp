#pragma once

#include "../mesh.hpp"
#include "../mesh_data.hpp"

class OpenGLMesh : public IMesh
{
public:
    OpenGLMesh(MeshData& meshData);
    ~OpenGLMesh();

    void draw(const IShader* shader, const Camera& camera) const override;

private:
    unsigned int vao;
    unsigned int ebo;

    unsigned int enableVertexBuffer(const std::vector<glm::vec2>& data, AttributeType attribute, bool useDynamicDraw);
    unsigned int enableVertexBuffer(const std::vector<glm::vec3>& data, AttributeType attribute, bool useDynamicDraw);
    unsigned int enableVertexBuffer(const std::vector<glm::vec4>& data, AttributeType attribute, bool useDynamicDraw);
};