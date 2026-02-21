#pragma once

#include "renderer.hpp"

class Renderable {
public:
    virtual ~Renderable() = default;
    virtual void createRenderResources(Renderer& renderer) = 0;
    virtual void syncRenderState() = 0; // usually sets model matrix
};
