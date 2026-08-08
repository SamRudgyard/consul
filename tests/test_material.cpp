#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <utility>

#include "graphics/material/material.hpp"
#include "graphics/texture/texture.hpp"

TEST_CASE("a material owns its textures")
{
    auto albedoTexture = std::make_shared<Texture>("albedo.png", TextureType::DIFFUSE);
    auto specularTexture = std::make_shared<Texture>("specular.png", TextureType::SPECULAR);
    auto normalTexture = std::make_shared<Texture>("normal.png", TextureType::NORMAL);
    std::weak_ptr<Texture> albedoTextureObserver = albedoTexture;
    std::weak_ptr<Texture> specularTextureObserver = specularTexture;
    std::weak_ptr<Texture> normalTextureObserver = normalTexture;

    Material material;
    material.setAlbedoTexture(std::move(albedoTexture));
    material.setSpecularTexture(std::move(specularTexture));
    material.setNormalTexture(std::move(normalTexture));

    REQUIRE_FALSE(albedoTextureObserver.expired());
    REQUIRE_FALSE(specularTextureObserver.expired());
    REQUIRE_FALSE(normalTextureObserver.expired());
}

TEST_CASE("replacing a material texture releases the previous texture")
{
    auto previousTexture = std::make_shared<Texture>("previous.png", TextureType::DIFFUSE);
    std::weak_ptr<Texture> previousTextureObserver = previousTexture;
    Material material;
    material.setAlbedoTexture(std::move(previousTexture));

    material.setAlbedoTexture(std::make_shared<Texture>("replacement.png", TextureType::DIFFUSE));

    REQUIRE(previousTextureObserver.expired());
}

TEST_CASE("a shared texture survives until its final material is destroyed")
{
    auto texture = std::make_shared<Texture>("shared.png", TextureType::DIFFUSE);
    std::weak_ptr<Texture> textureObserver = texture;
    auto firstMaterial = std::make_unique<Material>();
    auto secondMaterial = std::make_unique<Material>();
    firstMaterial->setAlbedoTexture(texture);
    secondMaterial->setAlbedoTexture(texture);
    texture.reset();

    firstMaterial.reset();
    REQUIRE_FALSE(textureObserver.expired());

    secondMaterial.reset();
    REQUIRE(textureObserver.expired());
}
