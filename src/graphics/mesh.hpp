#pragma once

#include "glm/glm.hpp"
#include "graphics/camera/camera.hpp"
#include "graphics/colour.hpp"
#include "graphics/texture.hpp"
#include "glad/glad.h"

#include <string>
#include <vector>
#include <iostream>
#include <utility>

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
    Mesh() = default;

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
        indexCount(indices.size()) {
            static unsigned int nextID = 0;
            id = nextID++;
        }

    /**
     * Gets the vertex positions associated with this Mesh.
     * @return Vertex positions.
     */
    const std::vector<glm::vec3>& getPositions() const { return positions; }
    
    /**
     * Gets the normals associated with this Mesh.
     * @return Normal vectors.
     */
    const std::vector<glm::vec3>& getNormals() const { return normals; }
    
    /**
     * Gets the texture coordinates associated with this Mesh.
     * @return Texture coordinates, in the form `(u, v)`.
     */
    const std::vector<glm::vec2>& getTextureCoords() const { return textureCoords; }
    
    /**
     * Gets the tangents associated with this Mesh.
     * @return Tangents.
     */
    const std::vector<glm::vec4>& getTangents() const { return tangents; }
    
    /**
     * Gets the indices associates with this Mesh.
     * If drawing using `DrawMode::LINES`, constructs three
     * pairs of indices for each line in a triangle.
     * 
     * @return Indices to draw the Mesh in currently set
     * draw mode.
     */
    std::vector<unsigned int> getIndices() const
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

    /**
     * Gets the textures associated with this Mesh.
     * @return Textures on this Mesh.
     */
    const std::vector<Texture>& getTextures() const { return textures; }

    /**
     * Sets the colour tint of this Mesh.
     * @param colour Colour tint.
     */
    void setTint(const Colour& colour) { tint = colour; }

    /**
     * Gets the colour tint of this Mesh.
     * @return Colour of the Mesh.
     */
    Colour getTint() const { return tint; }

    /**
     * Set the draw mode of this Mesh.
     * @param mode The draw mode.
     */
    void setDrawMode(DrawMode mode) { drawMode = mode; }

    /**
     * Gets the draw mode of this Mesh.
     * @return The draw mode.
     */
    DrawMode getDrawMode() const { return drawMode; }

    /**
     * Set the model matrix for this Mesh.
     * The model matrix converts from vertex attributes from local
     * model space to world space.
     * @param matrix The new model matrix.
     */
    void setModelMatrix(const glm::mat4& matrix) { modelMatrix = matrix; }

    /**
     * Gets the current model matrix for this Mesh.
     * The model matrix converts from vertex attributes from local
     * model space to world space.
     * @return The current model matrix.
     */
    const glm::mat4& getModelMatrix() const { return modelMatrix; }

    /**
     * Gets the number of indices associated with this Mesh.
     * @return Number of indices.
     */
    const unsigned int getNumIndices() const
    {
        if (drawMode == DrawMode::LINES) {
            // Each triangle (i0, i1, i2) becomes lines (i0, i1), (i1, i2), (i2, i0)
            // => x2 number of indices
            return 2*indexCount;
        }

        return indexCount;
    }

    /**
     * Determines if the Mesh has a given vertex attribute
     * (i.e. position, normals, texture coordinates, etc.) from a
     * given AttributeType enum.
     * @param type attribute type.
     * @return true if Mesh has this attribute type, false otherwise.
     */
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

    /**
     * Clears the vectors storing the Mesh's vertex attributes
     * (positions, normals, etc.) and its indices.
     */
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

    /**
     * Gets the unique ID of this Mesh.
     * @return ID of this Mesh.
     */
    unsigned int getID() const { return id; }

private:
    unsigned int id;
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
