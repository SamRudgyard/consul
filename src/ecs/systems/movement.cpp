#include "movement.hpp"

#include "../components/transform_2d.hpp"
#include "../components/physics_2d.hpp"

void MovementSystem::Update(float deltaTime) {
    entityManager->ForEach<Physics2D, Transform2D>([&](Physics2D& physics, Transform2D& transform) {
        transform.position += physics.velocity*deltaTime;
    });
}