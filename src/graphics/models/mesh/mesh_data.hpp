#pragma once

#include "glm/glm.hpp"
#include "graphics/camera/camera.hpp"
#include "graphics/textures/texture.hpp"

#include <string>
#include <vector>
#include <iostream>

class ITexture;

enum class AttributeType
{
    POSITION    = 0,
    NORMAL      = 1,
    COLOUR      = 2,
    TEXCOORD    = 3,
    TANGENT     = 4,
    INDICES     = 5,
};

class MeshData {
public:
    MeshData(
        std::vector<glm::vec3> positions,
        std::vector<glm::vec3> normals,
        std::vector<glm::vec4> colours,
        std::vector<glm::vec2> textureCoords,
        std::vector<glm::vec4> tangents,
        std::vector<unsigned int> indices,
        std::vector<TextureData> textures
    ) : positions(positions),
        normals(normals),
        colours(colours),
        textureCoords(textureCoords),
        tangents(tangents),
        indices(indices),
        textures(textures) {}

    void setVertexBuffer(unsigned int vbo, AttributeType type)
    {
        vertexBuffers[static_cast<size_t>(type)] = vbo;
    }

    unsigned int getVertexBuffer(AttributeType type) const
    {
        return vertexBuffers[static_cast<size_t>(type)];
    }

    const std::vector<glm::vec3>& getPositions() const { return positions; }
    const std::vector<glm::vec3>& getNormals() const { return normals; }
    const std::vector<glm::vec4>& getColours() const { return colours; }
    const std::vector<glm::vec2>& getTextureCoords() const { return textureCoords; }
    const std::vector<glm::vec4>& getTangents() const { return tangents; }
    const std::vector<unsigned int>& getIndices() const { return indices; }
    const std::vector<TextureData>& getTextures() const { return textures; }

private:
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec4> colours;
    std::vector<glm::vec2> textureCoords;
    std::vector<glm::vec4> tangents; // TODO: Why vec4 for tangents?
    std::vector<unsigned int> indices;
    std::vector<TextureData> textures;
    std::vector<unsigned int> vertexBuffers = std::vector<unsigned int>(6, 0);
};