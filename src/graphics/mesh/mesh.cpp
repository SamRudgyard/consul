#include "mesh.hpp"

Mesh::Mesh(
    std::vector<glm::vec3> positions,
    std::vector<glm::vec3> normals,
    std::vector<glm::vec2> textureCoords,
    std::vector<glm::vec4> tangents,
    std::vector<unsigned int> indices,
    std::vector<Texture> textures,
    Colour tint,
    DrawMode drawMode
) : positions(positions),
    normals(normals),
    textureCoords(textureCoords),
    tangents(tangents),
    indices(indices),
    textures(textures),
    tint(tint),
    drawMode(drawMode),
    indexCount(indices.size())
{
    static unsigned int nextID = 0;
    id = nextID++;

    // New meshes need an initial GPU upload of geometric attributes.
    uploadMask.set();
}

std::vector<unsigned int> Mesh::getIndices() const
{
    if (drawMode != DrawMode::LINES) {
        return indices;
    }

    std::vector<unsigned int> lineIndices;

    if (indexCount < 3) {
        // A point or line doesn't require conversion
        // from triangles to lines, so return
        return indices;
    }

    // Each triangle (i0, i1, i2) becomes lines (i0, i1), (i1, i2), (i2, i0)
    // => x2 number of indices
    lineIndices.reserve(2*indexCount);

    for (size_t i = 0; i < indices.size(); i += 3) {
        const unsigned int i0 = indices[i];
        const unsigned int i1 = indices[i + 1];
        const unsigned int i2 = indices[i + 2];

        lineIndices.push_back(i0);
        lineIndices.push_back(i1);
        lineIndices.push_back(i1);
        lineIndices.push_back(i2);
        lineIndices.push_back(i2);
        lineIndices.push_back(i0);
    }

    return lineIndices;
}

void Mesh::setDrawMode(DrawMode mode)
{
    if (drawMode != mode) {
        drawMode = mode;
        markDirty(AttributeType::INDICES);
    }
}

const unsigned int Mesh::getNumIndices() const
{
    if (drawMode == DrawMode::LINES) {
        // Each triangle (i0, i1, i2) becomes lines (i0, i1), (i1, i2), (i2, i0)
        // => x2 number of indices
        return 2*indexCount;
    }

    return indexCount;
}

bool Mesh::hasAttribute(AttributeType type) const
{
    switch (type) {
        case AttributeType::POSITION:
            return !positions.empty();
        case AttributeType::NORMAL:
            return !normals.empty();
        case AttributeType::TEXCOORD:
            return !textureCoords.empty();
        case AttributeType::TANGENT:
            return !tangents.empty();
        case AttributeType::INDICES:
            return !indices.empty();
        default:
            return false;
    }
}

void Mesh::clear()
{
    positions.clear();
    normals.clear();
    textureCoords.clear();
    tangents.clear();
    indices.clear();

    positions.shrink_to_fit();
    normals.shrink_to_fit();
    textureCoords.shrink_to_fit();
    tangents.shrink_to_fit();
    indices.shrink_to_fit();
}

bool Mesh::isAnyDirty() const
{
    return uploadMask.any();
}

bool Mesh::isDirty(AttributeType attribute) const
{
    return uploadMask.test((int)attribute);
}

void Mesh::markDirty(AttributeType attribute)
{
    uploadMask.set((int)attribute);
}

void Mesh::clean(AttributeType attribute)
{
    uploadMask.reset((int)attribute);
}