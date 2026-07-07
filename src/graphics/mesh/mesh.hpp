#pragma once

#include <bitset>
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <utility>

#include "glm/glm.hpp"
#include "graphics/camera/camera.hpp"
#include "graphics/colour.hpp"
#include "graphics/material/material.hpp"
#include "glad/glad.h"

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

using MeshUploadMask = std::bitset<(int)AttributeType::INDICES + 1>;

class Mesh {
public:
    Mesh() = default;

    Mesh(
        std::vector<glm::vec3> positions,
        std::vector<unsigned int> indices,
        DrawMode drawMode = DrawMode::TRIANGLES
    ) : Mesh(std::move(positions), {}, {}, {}, std::move(indices), drawMode) {}

    Mesh(
        std::vector<glm::vec3> positions,
        std::vector<glm::vec3> normals,
        std::vector<glm::vec2> textureCoords,
        std::vector<glm::vec4> tangents,
        std::vector<unsigned int> indices,
        DrawMode drawMode = DrawMode::TRIANGLES
    );

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
    std::vector<unsigned int> getIndices() const;

    /**
     * Sets the material for this Mesh.
     * @param material The new material for this Mesh.
     */
    void setMaterial(AssetID materialID) { this->materialID = materialID; }

    /**
     * Gets the material for this Mesh.
     * @return The material for this Mesh.
     */
    AssetID getMaterial() const { return materialID; }

    /**
     * Set the draw mode of this Mesh.
     * @param mode The draw mode.
     */
    void setDrawMode(DrawMode mode);

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
    const unsigned int getNumIndices() const;

    /**
     * Determines if the Mesh has a given vertex attribute
     * (i.e. position, normals, texture coordinates, etc.) from a
     * given AttributeType enum.
     * @param type attribute type.
     * @return true if Mesh has this attribute type, false otherwise.
     */
    bool hasAttribute(AttributeType type) const;

    /**
     * Clears the vectors storing the Mesh's vertex attributes
     * (positions, normals, etc.) and its indices.
     */
    void clear();

    /**
     * Gets the unique ID of this Mesh.
     * @return ID of this Mesh.
     */
    unsigned int getID() const { return id; }

    bool isAnyDirty() const;

    bool isDirty(AttributeType attribute) const;

    void markDirty(AttributeType attribute);

    void clean(AttributeType attribute);

private:
    unsigned int id; // TODO: Can this be removed? MeshAssetManager holds the asset ID.
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> textureCoords;
    std::vector<glm::vec4> tangents; // TODO: Why vec4 for tangents?
    std::vector<unsigned int> indices;
    AssetID materialID = INVALID_ASSET_ID;
    DrawMode drawMode = DrawMode::TRIANGLES;
    unsigned int indexCount = 0;
    MeshUploadMask uploadMask;
};
