#pragma once

#include <memory>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "nlohmann/json.hpp"

#include "core/project/asset_manager.hpp"
#include "core/project/asset_defaults.hpp"
#include "core/project/asset_types.hpp"
#include "graphics/mesh/mesh.hpp"

using json = nlohmann::json;

class Material;
class Model;

class GLTFImporter
{
public:
    GLTFImporter(
        ModelAssetManager& modelManager,
        MeshAssetManager& meshManager,
        MaterialAssetManager& materialManager,
        TextureAssetManager& textureManager,
        AssetDefaults& assetDefaults
    );
    ~GLTFImporter() = default;

    /**
     * Load a model from the given glTF file path.
     * @param name Name for the imported model asset.
     * @param filePath Path to the .gltf model file.
     * @returns Model loaded from the glTF file.
     */
    std::shared_ptr<Model> import(const std::string& name, const std::string& filePath);

private:
    ModelAssetManager& modelManager;
    MeshAssetManager& meshManager;
    MaterialAssetManager& materialManager;
    TextureAssetManager& textureManager;
    AssetDefaults& assetDefaults;

    Model* currentModel = nullptr;
    std::string currentFileDirectory = "";
    std::string currentFilePath = "";
    std::vector<unsigned char> binaryData;
    json jsonContents;

    void resetImportState();

    /**
	 * Traverse a node within the glTF file recursively to collect meshes and construct transforms.
	 * @param nextNode            Index of the node to traverse.
	 * @param parentTransMatrix   Transformation matrix of the parent node.
	 */
	void traverseNode(unsigned int nextNode, glm::mat4 parentTransMatrix = glm::mat4(1.0f));

	/**
	 * Read float attribute data referenced by an accessor.
	 * @param accessor JSON accessor object.
	 * @returns Vector of floats read from the accessor.
	 */
	std::vector<float> readAccessorFloats(json accessor);

	/**
	 * Read index attribute data referenced by an accessor.
	 * @param accessor JSON accessor object.
	 * @returns Vector of unsigned ints read from the accessor.
	 */
	std::vector<unsigned int> readAccessorIndices(json accessor);

	/**
	 * Retreive a texture's full path from a glTF texture index.
	 * @param textureIndex Index into the glTF textures array.
	 * @returns Full texture path.
	 */
	std::string getTexturePathFromUri(unsigned int textureIndex) const;

	/**
	 * Load a mesh from glTF mesh data.
	 * @param meshIndex Index into the glTF meshes array.
	 * @param initialTransform Initial transform matrix. Defaults to the identity matrix (i.e. no initial transform).
	 * @returns Loaded mesh.
	 */
	std::shared_ptr<Mesh> loadMesh(unsigned int meshIndex, const glm::mat4& initialTransform = glm::mat4(1.0f));

    std::shared_ptr<Mesh> addMesh(const std::string& name, const Mesh& mesh);

	/**
	 * Create a material from glTF material data.
	 * @param materialIndex Index into the glTF materials array.
	 * @returns Material with textures referenced by the glTF material.
	 */
	std::shared_ptr<Material> loadMaterial(int materialIndex);

    std::shared_ptr<Material> addMaterial(const std::string& name, const Material& material);

	/**
	 * Convert a float array to a vec2 array.
	 * @param floatVec Vector of floats.
	 * @returns Vector of vec2.
	 */
	std::vector<glm::vec2> toVec2(const std::vector<float> floatVec);

	/**
	 * Convert a float array to a vec3 array.
	 * @param floatVec Vector of floats.
	 * @returns Vector of vec3.
	 */
	std::vector<glm::vec3> toVec3(const std::vector<float> floatVec);

	/**
	 * Convert a float array to a vec4 array.
	 * @param floatVec Vector of floats.
	 * @returns Vector of vec4.
	 */
	std::vector<glm::vec4> toVec4(const std::vector<float> floatVec);
};
