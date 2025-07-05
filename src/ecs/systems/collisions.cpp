#include "collisions.hpp"

#include "../components/collisions.hpp"
#include "../components/transform_2d.hpp"
#include "../components/physics_2d.hpp"

void Collisions::Update(float deltaTime) {
    entityManager->ForEach<CollisionEdgeOfScreen, RectCollider, Transform2D, Physics2D>([&](CollisionEdgeOfScreen& collision, RectCollider& collider, Transform2D& transform, Physics2D& physics) {
        if (collision.isEnabled) {
            vec2 min = transform.position + collider.offset;
            vec2 max = min + collider.size;
            if (min.x <= 0.f || max.x >= 800.f) physics.velocity.x *= collision.reflectionMultiplier;
            if (min.y <= 0.f || max.y >= 600.f) physics.velocity.y *= collision.reflectionMultiplier;
        }
    });

    auto collisionEntities = entityManager->View<RectCollider, Transform2D, Physics2D>();
    for (unsigned int i = 0; i < collisionEntities.size(); i++) {
        for (unsigned int j = i + 1; j < collisionEntities.size(); j++) {
            Entity entityA = collisionEntities[i];
            Entity entityB = collisionEntities[j];

            vec2 collisionNormal;
            if (HasCollidedAABB(entityA, entityB, collisionNormal)) {
                Physics2D& physicsA = componentManager->GetComponent<Physics2D>(entityA);
                Physics2D& physicsB = componentManager->GetComponent<Physics2D>(entityB);

                if (collisionNormal.x == 1.f) {
                    physicsA.velocity.x *= -1.f;
                    physicsB.velocity.x *= -1.f;
                }
                if (collisionNormal.y == 1.f) {
                    physicsA.velocity.y *= -1.f;
                    physicsB.velocity.y *= -1.f;
                }
            }
        }
    }
}

bool Collisions::HasCollidedAABB(Entity entityA, Entity entityB, vec2& collisionNormal) {
    RectCollider& aRect = componentManager->GetComponent<RectCollider>(entityA);
    RectCollider& bRect = componentManager->GetComponent<RectCollider>(entityB);
    Transform2D& aTransform = componentManager->GetComponent<Transform2D>(entityA);
    Transform2D& bTransform = componentManager->GetComponent<Transform2D>(entityB);

    vec2 aMin = aTransform.position + aRect.offset;
    vec2 aMax = aMin + aRect.size;

    vec2 bMin = bTransform.position + bRect.offset;
    vec2 bMax = bMin + bRect.size;

    if (aMin.x < bMax.x && aMax.x > bMin.x &&
        aMin.y < bMax.y && aMax.y > bMin.y) {
        
        float overlapX = std::min(aMax.x, bMax.x) - std::max(aMin.x, bMin.x);
        float overlapY = std::min(aMax.y, bMax.y) - std::max(aMin.y, bMin.y);

        if (overlapX < overlapY) {
            collisionNormal = vec2(1.0f, 0.0f); // Horizontal collision
        } else {
            collisionNormal = vec2(0.0f, 1.0f); // Vertical collision
        }

        return true;
    }

    return false;
}
