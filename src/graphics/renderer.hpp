#pragma once

#include "core/console/console.hpp"
#include "graphics.hpp"
#include "graphics_opengl.hpp"
#include "graphics/mesh/mesh.hpp"
#include "graphics/mesh/renderable_mesh.hpp"
#include "graphics/models/model.hpp"
#include "graphics/camera/camera.hpp"
#include "graphics/shaders/shader.hpp"
#include "platforms/platform.hpp"
#include "imgui_impl_opengl3.h"
#include "utils.hpp"

class Renderer
{
public:
    Renderer(IPlatform* platform, GraphicsAPI gfxApi) : gfxApi(gfxApi) {
        switch (gfxApi) {
            case GraphicsAPI::OpenGL:
                gfxBackend = new OpenGLGraphics();
                break;
            default:
                Console::get().error("[Renderer] Unknown graphics API!");
                break;
        }

        if (platform && gfxBackend) {
            platform->loadGraphics(gfxBackend);
        }
    }

    ~Renderer() {
        delete gfxBackend;

        for (LoadedModel& loadedModel : loadedModels) {
            for (RenderableMesh* mesh : loadedModel.meshes) {
                delete mesh;
            }
            loadedModel.meshes.clear();
        }
        loadedModels.clear();

        for (RenderableMesh* mesh : standaloneMeshes) {
            delete mesh;
        }
        standaloneMeshes.clear();
    }

    void initialiseImGui()
    {
        switch (gfxApi) {
            case GraphicsAPI::OpenGL:
                ImGui_ImplOpenGL3_Init("#version 330");
                break;
            default:
                Console::get().error("[Renderer] Unknown graphics API for ImGui initialisation!");
                return;
        }
    }

    void clearBackground(const glm::vec4& colour)
    {
        if (!gfxBackend) {
            Console::get().error("[Renderer] Attempted to call clearBackground but no graphics backend exists!");
            return;
        }

        assert(gfxBackend != nullptr && "Graphics backend not initialized!");

        gfxBackend->clearColour(colour);
    }

    void setViewport(int x, int y, int width, int height)
    {
        gfxBackend->setViewport(x, y, width, height);
    }

    IShader* newShader(const std::string& vertexFilePath, const std::string& fragmentFilePath)
    {
        IShader* shader = gfxBackend->newShader();

        const std::string vertexSource = readFile(vertexFilePath.c_str());
        const std::string fragmentSource = readFile(fragmentFilePath.c_str());
        shader->compile(vertexSource, fragmentSource);
        return shader;
    }

    RenderableMesh* newMesh(Mesh& mesh)
    {
        RenderableMesh* renderableMesh = gfxBackend->newMesh(mesh);
        return renderableMesh;
    }

    RenderableMesh* addMesh(Mesh& mesh, const glm::mat4& modelMatrix = glm::mat4(1.0f))
    {
        RenderableMesh* renderableMesh = newMesh(mesh);
        if (renderableMesh) {
            renderableMesh->setModelMatrix(modelMatrix);
            standaloneMeshes.push_back(renderableMesh);
        }
        return renderableMesh;
    }

    void loadModel(Model& model)
    {
        std::vector<Mesh> meshes = model.getMeshes();
        std::vector<glm::mat4> transforms = model.getTransformationMatrices();
        LoadedModel loadedModel{&model, {}};
        loadedModel.meshes.reserve(meshes.size());
        for (unsigned int iMesh = 0; iMesh < meshes.size(); iMesh++) {
            Mesh& mesh = meshes[iMesh];
            RenderableMesh* renderableMesh = newMesh(mesh);
            if (iMesh < transforms.size()) {
                renderableMesh->setModelMatrix(transforms[iMesh]);
            } else {
                Console::get().warn("[Renderer::loadModel] Missing transform for mesh index: '" + std::to_string(iMesh) + "'");
            }
            loadedModel.meshes.push_back(renderableMesh);
        }
        loadedModels.push_back(loadedModel);
    }

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

    
private:
    struct LoadedModel {
        Model* model;
        std::vector<RenderableMesh*> meshes;
    };

    IGraphics* gfxBackend = nullptr;
    GraphicsAPI gfxApi;
    std::vector<LoadedModel> loadedModels;
    std::vector<RenderableMesh*> standaloneMeshes;
};
