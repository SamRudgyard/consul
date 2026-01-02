#pragma once

#include "glm/glm.hpp"
#include "graphics/camera/camera.hpp"
#include "graphics/colour.hpp"
#include "graphics/textures/renderable_texture.hpp"

#include <string>
#include <vector>
#include <iostream>

class RenderableTexture;

enum class AttributeType
{
    POSITION    = 0,
    NORMAL      = 1,
    TEXCOORD    = 2,
    TANGENT     = 3,
    INDICES     = 4,
};

class Mesh {
public:
    Mesh(
        std::vector<glm::vec3> positions,
        std::vector<glm::vec3> normals,
        std::vector<glm::vec2> textureCoords,
        std::vector<glm::vec4> tangents,
        std::vector<unsigned int> indices,
        std::vector<Texture> textures,
        Colour tint = Colour(255, 255, 255, 255)
    ) : positions(positions),
        normals(normals),
        textureCoords(textureCoords),
        tangents(tangents),
        indices(indices),
        textures(textures),
        tint(tint),
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
    const std::vector<glm::vec2>& getTextureCoords() const { return textureCoords; }
    const std::vector<glm::vec4>& getTangents() const { return tangents; }
    const std::vector<unsigned int>& getIndices() const { return indices; }
    const unsigned int getIndexCount() const { return indexCount; }
    const std::vector<Texture>& getTextures() const { return textures; }
    Colour getTint() const { return tint; }
    void setTint(const Colour& value) { tint = value; }

    void setModelMatrix(const glm::mat4& matrix) { modelMatrix = matrix; }
    const glm::mat4& getModelMatrix() const { return modelMatrix; }

    const unsigned int getNumIndices() const { return (unsigned int)indexCount; }

    void clear()
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

private:
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> textureCoords;
    std::vector<glm::vec4> tangents; // TODO: Why vec4 for tangents?
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
    Colour tint = Colour(255, 255, 255, 255);
    unsigned int indexCount = 0;
    std::vector<unsigned int> vertexBuffers = std::vector<unsigned int>(5, 0);
};
