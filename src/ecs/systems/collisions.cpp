#include "collisions.hpp"

#include "../components/collisions.hpp"
#include "../components/transform_2d.hpp"
#include "../components/physics_2d.hpp"

void Collisions::Update(float deltaTime) {
    entityManager->ForEach<CollisionEdgeOfScreen, Transform2D, Physics2D>([&](CollisionEdgeOfScreen& collision, Transform2D& transform, Physics2D& physics) {
        if (collision.isEnabled) {
            if (transform.position.x <= 0.f || transform.position.x >= 800.f) {
                physics.velocity.x *= collision.reflectionMultiplier;
                Log("Collision detected!");
            }
            if (transform.position.y <= 0.f || transform.position.y >= 600.f) physics.velocity.y *= collision.reflectionMultiplier;
        }
    });
}