#pragma once

#include "../system_manager.hpp"

#include "SDL3/SDL.h"
#include "glm/glm.hpp"

using namespace glm;

class InputHandler : public IUpdateSystem {
private:
    vec2 direction;
    void DetectMovement();
public:
    InputHandler();
    void Update(float deltaTime) override;
};