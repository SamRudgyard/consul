#pragma once

#include "mesh.hpp"

class RenderableMesh
{
public:
    RenderableMesh(Mesh& mesh) : mesh(mesh) {}
    virtual ~RenderableMesh() = default;

    virtual void draw(const IShader* shader, const Camera& camera) const = 0;

    void setModelMatrix(const glm::mat4& matrix) { mesh.setModelMatrix(matrix); }
    const glm::mat4& getModelMatrix() const { return mesh.getModelMatrix(); }
protected:
    Mesh mesh;
};
