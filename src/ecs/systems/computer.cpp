#include "computer.hpp"

#include <vector>

#include "glm/glm.hpp"

#include "../components/transform_2d.hpp"
#include "../components/physics_2d.hpp"
#include "../components/tags.hpp"

using namespace std;
using namespace glm;

void ComputerSystem::Update(float deltaTime) {
    vector<Entity> ballEntities = entityManager->View<BallTag>();
    if (ballEntities.size() == 0) return;

    Entity ballEntity = ballEntities[0];
    vec2 ballPosition = componentManager->GetComponent<Transform2D>(ballEntity).position;

    entityManager->ForEach<Transform2D, Physics2D, ComputerTag>([&](Transform2D& transform, Physics2D& physics, ComputerTag& tag) {
        float dy = ballPosition.y - transform.position.y;
        if (dy == 0.f) return;
        physics.acceleration = physics.oneMass*vec2(0.f, dy/abs(dy));
    });
}