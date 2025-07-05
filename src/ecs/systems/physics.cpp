#include "physics.hpp"

#include "glm/glm.hpp"

#include "../components/physics_2d.hpp"
#include "../components/player_controller.hpp"

using namespace glm;

void PhysicsSystem::Update(float deltaTime) {
    entityManager->ForEach<Physics2D, PlayerController>([&](Physics2D& physics, PlayerController& controller) {
        if (controller.inputDirection == vec2(0.f, 0.f)) {
            physics.acceleration = -physics.velocity * physics.coefficientOfFriction;
        } else {
            physics.acceleration = physics.speed * controller.inputDirection;
        }
    });

    entityManager->ForEach<Physics2D>([&](Physics2D& physics) {
        physics.velocity += physics.acceleration*deltaTime;
    });
}