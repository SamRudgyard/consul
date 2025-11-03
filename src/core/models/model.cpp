#include "core/models/model.hpp"
#include "core/console/console.hpp"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "shader.hpp"

void Model::loadModel(const std::string& path)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        Console::Get().Error("[Consul] [Model::loadModel] Error: " + std::string(importer.GetErrorString()));
        return;
    }
    
    directory = path.substr(0, path.find_last_of('/'));

    // process assimp's root node recursively
    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
    // process all the node's meshes (if any)
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    // then do the same for each of its children
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }

}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;

        glm::vec3 vector;
        vector.x = scale.x*mesh->mVertices[i].x;
        vector.y = scale.y*mesh->mVertices[i].y;
        vector.z = scale.z*mesh->mVertices[i].z;
        vertex.position = vector;
        
        if (mesh->HasNormals())
        {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.normal = vector;
        }

        vertex.colour = glm::vec3(1.0f, 1.0f, 1.0f);
        
        if (mesh->mTextureCoords[0])
        {
            glm::vec2 vec;
            // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vec.x = mesh->mTextureCoords[0][i].x; 
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.textureCoords = vec;
        }
        else
        {
            vertex.textureCoords = glm::vec2(0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }
    // now walk through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<Texture> diffuseMaps = loadMaterialTextures(material, TextureType::DIFFUSE);
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        std::vector<Texture> specularMaps = loadMaterialTextures(material, TextureType::SPECULAR);
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }
    
    // return a mesh object created from the extracted mesh data
    return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, TextureType type) {
    std::vector<Texture> textures;

    aiTextureType assimpType = type == TextureType::DIFFUSE  ? aiTextureType_DIFFUSE : aiTextureType_SPECULAR;

    for (unsigned int it = 0; it < mat->GetTextureCount(assimpType); it++) {
        aiString str;
        mat->GetTexture(assimpType, it, &str);

        std::string textureFullPath = directory + '/' + str.C_Str();

        bool skip = false;
        for (unsigned int il = 0; il < loadedTextures.size(); il++) {
            if (strcmp(loadedTextures[il].path.c_str(),textureFullPath.c_str()) == 0) {
                textures.push_back(loadedTextures[il]);
                skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                break;
            }
        }
        if (!skip) { // if texture hasn't been loaded already, load it
            Texture texture(textureFullPath.c_str(), type, it);
            textures.push_back(texture);
            loadedTextures.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
        }
    }

    return textures;
}

void Model::Draw(Shader& shader, Camera& camera) const
{
    for (const auto& mesh : meshes) {
        mesh.Draw(shader, camera);
    }
}