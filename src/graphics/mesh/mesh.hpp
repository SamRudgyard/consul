#pragma once

#include "glm/glm.hpp"
#include "graphics/camera/camera.hpp"
#include "graphics/textures/renderable_texture.hpp"

#include <string>
#include <vector>
#include <iostream>

class RenderableTexture;

enum class AttributeType
{
    POSITION    = 0,
    NORMAL      = 1,
    COLOUR      = 2,
    TEXCOORD    = 3,
    TANGENT     = 4,
    INDICES     = 5,
};

class Mesh {
public:
    Mesh(
        std::vector<glm::vec3> positions,
        std::vector<glm::vec3> normals,
        std::vector<glm::vec4> colours,
        std::vector<glm::vec2> textureCoords,
        std::vector<glm::vec4> tangents,
        std::vector<unsigned int> indices,
        std::vector<Texture> textures
    ) : positions(positions),
        normals(normals),
        colours(colours),
        textureCoords(textureCoords),
        tangents(tangents),
        indices(indices),
        textures(textures),
        indexCount(indices.size()) {}

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
    const unsigned int getIndexCount() const { return indexCount; }
    const std::vector<Texture>& getTextures() const { return textures; }

    void setModelMatrix(const glm::mat4& matrix) { modelMatrix = matrix; }
    const glm::mat4& getModelMatrix() const { return modelMatrix; }

    const unsigned int getNumIndices() const { return (unsigned int)indexCount; }

    void clear()
    {
        positions.clear();
        normals.clear();
        colours.clear();
        textureCoords.clear();
        tangents.clear();
        indices.clear();

        positions.shrink_to_fit();
        normals.shrink_to_fit();
        colours.shrink_to_fit();
        textureCoords.shrink_to_fit();
        tangents.shrink_to_fit();
        indices.shrink_to_fit();
    }

private:
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec4> colours;
    std::vector<glm::vec2> textureCoords;
    std::vector<glm::vec4> tangents; // TODO: Why vec4 for tangents?
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
    unsigned int indexCount = 0;
    std::vector<unsigned int> vertexBuffers = std::vector<unsigned int>(6, 0);
};
