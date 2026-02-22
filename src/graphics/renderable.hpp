#pragma once

#include "renderer.hpp"

class Renderable {
public:
    virtual ~Renderable() = default;

    /**
     * Initialise any rendering resources required for this renderable
     * (e.g. create a `RenderableMesh` from a `Mesh`).
     * @param renderer Renderer to use for resource creation.
     */
    virtual void initRendering(Renderer& renderer) = 0;

    /**
     * Sync any relevant data to the renderer (e.g. update model matrix).
     * Called every frame before rendering.
     */
    virtual void syncToRenderer() = 0; // usually sets model matrix
};
