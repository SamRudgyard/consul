#pragma once

#include "../entity_manager.hpp"
#include "../component_manager.hpp"
#include "../components/transform_2d.hpp"
#include "../components/rectangle.hpp"

#include "SDL3/SDL.h"

class DrawSystem {
public:
    DrawSystem(SDL_Renderer* renderer)
        : renderer(renderer), entityManager(EntityManager::GetInstance()), componentManager(ComponentManager::GetInstance()) {}

    void Draw();

private:
    SDL_Renderer* renderer;
    EntityManager* entityManager;
    ComponentManager* componentManager;
};