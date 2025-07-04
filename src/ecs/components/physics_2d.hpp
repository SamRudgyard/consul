#pragma once

#include "glm/glm.hpp"

using namespace glm;


class Physics2D {
private:
    float speed = 0.f;
    vec2 velocity = vec2(0.f, 0.f);
    vec2 acceleration = vec2(0.f, 0.f);
    float coefficientOfFriction = 0.f;
public:
    Physics2D() = default;
    Physics2D(float speed, vec2 velocity, vec2 acceleration) : speed(speed), velocity(velocity), acceleration(acceleration) {}

    void SetSpeed(float speed) { this->speed = speed; }
    void SetVelocity(vec2 velocity) { this->velocity = velocity; }
    void SetAcceleration(vec2 acceleration) { this->acceleration = acceleration; }
    void SetCoefficientOfFriction(float coefficientOfFriction) { this->coefficientOfFriction = coefficientOfFriction; }

    float GetSpeed() { return speed; }
    vec2 GetVelocity() { return velocity; }
    vec2 GetAcceleration() { return acceleration; }
    float GetCoefficientOfFriction() { return coefficientOfFriction; }
};