#include "movement.hpp"

#include "../components/transform_2d.hpp"
#include "../components/physics_2d.hpp"
#include "../components/player_controller.hpp"

void MovementSystem::Update(float deltaTime) {
    vector<Entity> entities = entityManager->View<Physics2D, PlayerController>();

    for (Entity entity : entities) {
        Physics2D& physics = componentManager->GetComponent<Physics2D>(entity);
        PlayerController& playerController = componentManager->GetComponent<PlayerController>(entity);

        if (playerController.inputDirection == vec2(0.f, 0.f)) {
            physics.acceleration = -physics.velocity*physics.coefficientOfFriction;
            continue;
        }

        physics.acceleration = physics.speed*playerController.inputDirection;
    }

    entities = entityManager->View<Transform2D, Physics2D>();

    for (Entity entity : entities) {
        Transform2D& transform = componentManager->GetComponent<Transform2D>(entity);
        Physics2D& physics = componentManager->GetComponent<Physics2D>(entity);

        physics.velocity += physics.acceleration*deltaTime;
        transform.position += physics.velocity*deltaTime;
    }
}