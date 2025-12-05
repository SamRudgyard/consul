#pragma once

#include "mesh_data.hpp"

class RenderableMesh
{
public:
    RenderableMesh(MeshData& meshData) : meshData(meshData) {}
    virtual ~RenderableMesh() = default;

    virtual void draw(const IShader* shader, const Camera& camera) const = 0;

    const MeshData& getMeshData() const { return meshData; }

    void setModelMatrix(const glm::mat4& matrix) { meshData.setModelMatrix(matrix); }
    const glm::mat4& getModelMatrix() const { return meshData.getModelMatrix(); }
private:
    MeshData meshData;
};