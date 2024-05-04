#pragma once

#include "glm/vec2.hpp"
#include "actor.h"
class Projectile : public Actor {
    public:
    glm::vec2 velocity;

    Projectile(const std::string& name, std::shared_ptr<PhysicsBody> body, glm::vec2 vel) : Actor(name, body){velocity = vel;}

void physics(float delta, const std::shared_ptr<World>& world) override {
    // TODO MOVE ON PATH
    std::cout << this->getPos().x << std::endl;
    bool collided = false;
    *this += glm::vec2(velocity.x / 3* delta, 0);
    if (glm::length(velocity) && world->checkCollisions(world->staticBodies, *body)){
        collided = true;
    }

    *this += glm::vec2(0, velocity.y / 3* delta);
    if (glm::length(velocity) && world->checkCollisions(world->staticBodies, *body)){
        collided = true;
    }
    if (collided) {

        world->actors.erase(this->name);
    }
}
};
