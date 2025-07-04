#include "collisions.hpp"

#include "glm/glm.hpp"

#include "../components/collisions.hpp"
#include "../components/transform_2d.hpp"
#include "../components/physics_2d.hpp"

using namespace glm;

void Collisions::Update(float deltaTime) {
    entityManager->ForEach<CollisionEdgeOfScreen, Transform2D, Physics2D>([&](CollisionEdgeOfScreen& collision, Transform2D& transform, Physics2D& physics) {
        if (collision.isEnabled) {
            if (transform.position.x <= 0.f || transform.position.x >= 800.f) physics.velocity.x *= collision.reflectionMultiplier;
            if (transform.position.y <= 0.f || transform.position.y >= 600.f) physics.velocity.y *= collision.reflectionMultiplier;
        }
    });

    auto collisionEntities = entityManager->View<RectCollider, Transform2D, Physics2D>();
    for (unsigned int i = 0; i < collisionEntities.size(); i++) {
        for (unsigned int j = i + 1; j < collisionEntities.size(); j++) {
            Entity entityA = collisionEntities[i];
            Entity entityB = collisionEntities[j];
            if (HasCollidedAABB(entityA, entityB)) {
                Physics2D& physicsA = componentManager->GetComponent<Physics2D>(entityA);
                physicsA.velocity *= -1.f;
                Physics2D& physicsB = componentManager->GetComponent<Physics2D>(entityB);
                physicsB.velocity *= -1.f;
            }
        }
    }
}

bool Collisions::HasCollidedAABB(Entity entityA, Entity entityB) {
    RectCollider& aRect = componentManager->GetComponent<RectCollider>(entityA);
    RectCollider& bRect = componentManager->GetComponent<RectCollider>(entityB);
    Transform2D& aTransform = componentManager->GetComponent<Transform2D>(entityA);
    Transform2D& bTransform = componentManager->GetComponent<Transform2D>(entityB);

    vec2 aMin = aTransform.position + aRect.offset;
    vec2 aMax = aMin + aRect.size;

    vec2 bMin = bTransform.position + bRect.offset;
    vec2 bMax = bMin + bRect.size;

    return (aMin.x < bMax.x && 
            aMax.x > bMin.x &&
            aMin.y < bMax.y && 
            aMax.y > bMin.y);
}