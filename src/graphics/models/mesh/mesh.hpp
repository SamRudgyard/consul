#pragma once

#include "mesh_data.hpp"

class IMesh
{
public:
    IMesh(MeshData& meshData) : meshData(meshData) {}
    virtual ~IMesh() = default;

    virtual void draw(IShader* shader, Camera& camera) const = 0;

    const MeshData& getMeshData() const { return meshData; }
private:
    MeshData meshData;
};