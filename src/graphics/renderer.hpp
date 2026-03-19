#pragma once

#include "core/console/console.hpp"
#include "graphics/mesh.hpp"
#include "graphics/models/model.hpp"
#include "graphics/camera/camera.hpp"
#include "graphics/shader.hpp"
#include "platforms/platform.hpp"
#include "imgui_impl_opengl3.h"
#include "utils.hpp"

enum class GraphicsAPI
{
    OpenGL,
    // Future graphics APIs can be added here (e.g., Vulkan, DirectX, etc.)
};

class Renderer
{
public:
    Renderer(Platform* platform) {};
    virtual ~Renderer() = default;

    virtual void initialiseGraphics(void* loaderFunc) = 0;

    virtual void initialiseImGui() = 0;

    virtual void clearBackground(const glm::vec4& colour) = 0;

    virtual void clearScreenBuffer() = 0;

    virtual void setViewport(int x, int y, int width, int height) = 0;

    virtual void uploadShader(const Shader& shader) = 0;

    virtual void uploadMesh(const Mesh& mesh) = 0;

    virtual void uploadTexture(const Texture& texture) = 0;

    virtual void uploadModel(Model& model) = 0;

    void render(const IShader* shader, const Camera& camera)
    {
        for (RenderableMesh* mesh : standaloneMeshes) {
            mesh->draw(shader, camera);
        }

        for (LoadedModel& loadedModel : loadedModels) {
            std::vector<glm::mat4> transforms = loadedModel.model->getTransformationMatrices();
            for (unsigned int iMesh = 0; iMesh < loadedModel.meshes.size(); iMesh++) {
                if (iMesh < transforms.size()) {
                    loadedModel.meshes[iMesh]->setModelMatrix(transforms[iMesh]);
                }
                loadedModel.meshes[iMesh]->draw(shader, camera);
            }
        }
    }
};
