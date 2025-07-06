#include "input_handler.hpp"

#include "SDL3/SDL.h"

#include "../components/physics_2d.hpp"
#include "../components/tags.hpp"

void InputHandler::Update(float deltaTime) {
    DetectPlayerMovement();
    DetectTogglePause();
}

void InputHandler::DetectPlayerMovement() {
    vec2 direction = vec2(0.f, 0.f);

    const bool* keyboardState = SDL_GetKeyboardState(NULL);
    if (keyboardState[SDL_SCANCODE_W]) direction += vec2(0.f, -1.f);
    if (keyboardState[SDL_SCANCODE_S]) direction += vec2(0.f, 1.f);
    if (keyboardState[SDL_SCANCODE_A]) direction += vec2(-1.f, 0.f);
    if (keyboardState[SDL_SCANCODE_D]) direction += vec2(1.f, 0.f);

    // Normalize direction
    if (direction.x != 0.f || direction.y != 0.f) {
        float length = sqrt(direction.x*direction.x + direction.y*direction.y);
        direction = direction/length;
    }

    entityManager->ForEach<Physics2D, PlayerTag>([&](Physics2D& physics, PlayerTag& tag) {
        physics.acceleration = physics.oneMass*direction;
    });
}

void InputHandler::DetectTogglePause() {
    static bool wasSpacePressed = false;
    const bool* keyboardState = SDL_GetKeyboardState(NULL);

    if (keyboardState[SDL_SCANCODE_SPACE]) {
        if (!wasSpacePressed) {
            if (appManager->state == AppState::Paused) {
                appManager->state = AppState::Running;
            } else if (appManager->state == AppState::Running) {
                appManager->state = AppState::Paused;
            }
        }
        wasSpacePressed = true;
    } else {
        wasSpacePressed = false;
    }
}