#include "input_handler.hpp"

#include "SDL3/SDL.h"
#include "../entity_manager.hpp"
#include "../component_manager.hpp"

#include "../components/transform_2d.hpp"
#include "../components/player_controller.hpp"


InputHandler::InputHandler() {

}

void InputHandler::Update(float deltaTime) {
    DetectMovement();

    EntityManager* entityManager = EntityManager::GetInstance();
    ComponentManager* componentManager = ComponentManager::GetInstance();

    entityManager->ForEach<Transform2D, PlayerController>([&](Transform2D& transform, PlayerController& playerController) {
        playerController.inputDirection = direction;
    });
}

void InputHandler::DetectMovement() {
    const bool* keyboardState = SDL_GetKeyboardState(NULL);

    direction = vec2(0.f, 0.f);

    if (keyboardState[SDL_SCANCODE_W]) direction += vec2(0.f, -1.f);
    if (keyboardState[SDL_SCANCODE_S]) direction += vec2(0.f, 1.f);
    if (keyboardState[SDL_SCANCODE_A]) direction += vec2(-1.f, 0.f);
    if (keyboardState[SDL_SCANCODE_D]) direction += vec2(1.f, 0.f);
}