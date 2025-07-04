#pragma once

#include "../entity_manager.hpp"
#include "../component_manager.hpp"
#include "../system_manager.hpp"
#include "../components/transform_2d.hpp"
#include "../components/rectangle.hpp"

#include "SDL3/SDL.h"

class RenderSystem : public IRenderSystem {
public:
    RenderSystem(SDL_Renderer* renderer)
        : renderer(renderer), entityManager(EntityManager::GetInstance()), componentManager(ComponentManager::GetInstance()) {}

    void Render() override;

private:
    SDL_Renderer* renderer;
    EntityManager* entityManager;
    ComponentManager* componentManager;
};