#pragma once

#include "core/engine_context.hpp"
#include "graphics/shaders/shader.hpp"

class Camera {
public:
    virtual void handleInputs(float deltaTime) = 0;

    virtual void sendToShader(const IShader* shader) const = 0;
protected:
    EngineContext* context = EngineContext::get();
};