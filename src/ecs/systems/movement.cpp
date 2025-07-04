#include "movement.hpp"

#include "../components/transform_2d.hpp"
#include "../components/physics_2d.hpp"
#include "../components/player_controller.hpp"

void MovementSystem::Update(float deltaTime) {
    entityManager->ForEach<Physics2D, PlayerController>([=](Physics2D& physics, PlayerController& controller) {
        if (controller.inputDirection == vec2(0.f, 0.f)) {
            physics.acceleration = -physics.velocity * physics.coefficientOfFriction;
        } else {
            physics.acceleration = physics.speed * controller.inputDirection;
        }
    });

    entityManager->ForEach<Physics2D, Transform2D>([=](Physics2D& physics, Transform2D& transform) {
        physics.velocity += physics.acceleration*deltaTime;
        transform.position += physics.velocity*deltaTime;
    });
}