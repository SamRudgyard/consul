#include "texture_manager.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "glad/glad.h"

#include "utils.hpp"

std::map<std::string, std::shared_ptr<Texture>> TextureManager::textures;

std::shared_ptr<Texture> TextureManager::LoadTexture(const char* filePath, const char* referenceName, bool requiresAlphaChannel)
{
    // if (!filePath) Error("[TextureManager::LoadTexture] Texture file path is null");

    // Check if name is already present
    // if (textures.find(referenceName) != textures.end()) LogOnDebug("[TextureManager::LoadTexture] Texture with name '" + std::string(referenceName) + "' is already loaded, will overwrite.");

    // if (!DoesFileExist(filePath)) Error("[TextureManager::LoadTexture] Texture file does not exist: '" + std::string(filePath) + "'");

    auto texture = std::make_shared<Texture>();

    if (requiresAlphaChannel) {
        texture->internalFormat = GL_RGBA;
        texture->imageFormat = GL_RGBA;
    } else {
        texture->internalFormat = GL_RGB;
        texture->imageFormat = GL_RGB;
    }

    int width, height, nrChannels;
    unsigned char* data = stbi_load(filePath, &width, &height, &nrChannels, 0);
    texture->Generate(width, height, data);
    stbi_image_free(data);

    textures[referenceName] = texture;

    // LogOnDebug("[TextureManager::LoadTexture] Loaded texture from file '" + std::string(filePath) + "' (ID: " + std::to_string(texture->id) + ", Size: " + std::to_string(width) + "x" + std::to_string(height) + ")");

    return textures[referenceName];
}


std::shared_ptr<Texture> TextureManager::GetTexture(const char* referenceName)
{
    if (textures.find(referenceName) == textures.end()) {
        // Error("[TextureManager::GetTexture] Texture with name '" + std::string(referenceName) + "' is not loaded");
        return nullptr;
    }

    return textures[referenceName];
}


void TextureManager::UnloadTexture(std::shared_ptr<Texture> texture)
{
    if (!texture) return;
    glDeleteTextures(1, &texture->id);

    // Remove from map
    for (auto it = textures.begin(); it != textures.end(); ++it) {
        if (it->second && it->second->id == texture->id) {
            textures.erase(it);
            break;
        }
    }

    // LogOnDebug("[TextureManager::UnloadTexture] Unloaded texture with ID " + std::to_string(texture->id));
}


void TextureManager::Clear()
{
    for (auto& pair : textures) {
        if (pair.second) {
            glDeleteTextures(1, &pair.second->id);
        }
    }
    textures.clear();
    // LogOnDebug("[TextureManager::Clear] Cleared all textures from memory");
}