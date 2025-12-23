#pragma once

#include <map>
#include <memory>

#include "../renderable_mesh.hpp"
#include "../mesh.hpp"
#include "graphics/textures/opengl/opengl_texture.hpp"

class OpenGLMesh : public RenderableMesh
{
public:
    OpenGLMesh(Mesh& mesh);
    ~OpenGLMesh();

    void draw(const IShader* shader, const Camera& camera) const override;

private:
    unsigned int vao;
    unsigned int ebo;
    std::map<std::string, std::shared_ptr<OpenGLTexture>> textures;

    unsigned int enableVertexBuffer(const std::vector<glm::vec2>& data, AttributeType attribute, bool useDynamicDraw);
    unsigned int enableVertexBuffer(const std::vector<glm::vec3>& data, AttributeType attribute, bool useDynamicDraw);
    unsigned int enableVertexBuffer(const std::vector<glm::vec4>& data, AttributeType attribute, bool useDynamicDraw);
};
