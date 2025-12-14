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

        for (RenderableMesh* mesh : loadedMeshes) {
            delete mesh;
        }
        loadedMeshes.clear();
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

    void loadModel(Model& model)
    {
        std::vector<Mesh> meshes = model.getMeshes();
        for (Mesh& mesh : meshes)
        {
            RenderableMesh* renderableMesh = newMesh(mesh);
            loadedMeshes.push_back(renderableMesh);
        }
    }

    void render(const IShader* shader, const Camera& camera)
    {
        for (RenderableMesh* mesh : loadedMeshes)
        {
            mesh->draw(shader, camera);
        }
    }

    
private:
    IGraphics* gfxBackend = nullptr;
    GraphicsAPI gfxApi;
    std::vector<RenderableMesh*> loadedMeshes;
};