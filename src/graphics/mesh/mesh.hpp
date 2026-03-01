#pragma once

#include "glm/glm.hpp"
#include "graphics/camera/camera.hpp"
#include "graphics/colour.hpp"
#include "graphics/textures/renderable_texture.hpp"
#include "glad/glad.h"

#include <string>
#include <vector>
#include <iostream>
#include <utility>

class RenderableTexture;

enum class AttributeType
{
    POSITION    = 0,
    NORMAL      = 1,
    TEXCOORD    = 2,
    TANGENT     = 3,
    INDICES     = 4,
};

enum class DrawMode
{
    TRIANGLES,
    LINES
};

class Mesh {
public:
    Mesh(
        std::vector<glm::vec3> positions,
        std::vector<unsigned int> indices,
        std::vector<Texture> textures = {},
        Colour tint = Colour(255, 255, 255, 255),
        DrawMode drawMode = DrawMode::TRIANGLES
    ) : Mesh(std::move(positions), {}, {}, {}, std::move(indices), std::move(textures), tint, drawMode) {}

    Mesh(
        std::vector<glm::vec3> positions,
        std::vector<glm::vec3> normals,
        std::vector<glm::vec2> textureCoords,
        std::vector<glm::vec4> tangents,
        std::vector<unsigned int> indices,
        std::vector<Texture> textures,
        Colour tint = Colour(255, 255, 255, 255),
        DrawMode drawMode = DrawMode::TRIANGLES
    ) : positions(positions),
        normals(normals),
        textureCoords(textureCoords),
        tangents(tangents),
        indices(indices),
        textures(textures),
        tint(tint),
        drawMode(drawMode),
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
    std::vector<unsigned int> getIndices() const
    {
        if (drawMode != DrawMode::LINES) {
            return indices;
        }

        std::vector<unsigned int> lineIndices;
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
    const std::vector<Texture>& getTextures() const { return textures; }
    Colour getTint() const { return tint; }
    void setTint(const Colour& value) { tint = value; }

    DrawMode getDrawMode() const { return drawMode; }
    void setDrawMode(DrawMode mode) { drawMode = mode; }

    void setModelMatrix(const glm::mat4& matrix) { modelMatrix = matrix; }
    const glm::mat4& getModelMatrix() const { return modelMatrix; }

    const unsigned int getNumIndices() const
    {
        if (drawMode == DrawMode::LINES) {
            // Each triangle (i0, i1, i2) becomes lines (i0, i1), (i1, i2), (i2, i0)
            // => x2 number of indices
            return 2*indexCount;
        }

        return indexCount;
    }

    bool hasAttribute(AttributeType type) const
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
    DrawMode drawMode = DrawMode::TRIANGLES;
    unsigned int indexCount = 0;
    std::vector<unsigned int> vertexBuffers = std::vector<unsigned int>(5, 0);
};
