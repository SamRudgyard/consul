#include "physics.hpp"

#include "glm/glm.hpp"

#include "../components/physics_2d.hpp"

using namespace glm;

void PhysicsSystem::Update(float deltaTime) {
    entityManager->ForEach<Physics2D>([&](Physics2D& physics) {
        physics.acceleration -= physics.velocity*physics.coefficientOfFriction;
        physics.velocity += physics.acceleration*deltaTime;
    });
}