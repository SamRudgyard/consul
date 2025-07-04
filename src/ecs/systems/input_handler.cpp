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

    vector<Entity> entities = entityManager->View<Transform2D, PlayerController>();

    for (Entity entity : entities) {
        PlayerController& playerController = componentManager->GetComponent<PlayerController>(entity);
        playerController.SetInputDirection(direction);
    }

    Log("Input: " + to_string(direction.x) + ", " + to_string(direction.y));
}

void InputHandler::DetectMovement() {
    const bool* keyboardState = SDL_GetKeyboardState(NULL);

    direction = vec2(0.f, 0.f);

    if (keyboardState[SDL_SCANCODE_W]) direction += vec2(0.f, -1.f);
    if (keyboardState[SDL_SCANCODE_S]) direction += vec2(0.f, 1.f);
    if (keyboardState[SDL_SCANCODE_A]) direction += vec2(-1.f, 0.f);
    if (keyboardState[SDL_SCANCODE_D]) direction += vec2(1.f, 0.f);
}