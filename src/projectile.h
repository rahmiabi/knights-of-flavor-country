#pragma once

#include "glm/vec2.hpp"
#include "actor.h"
class Projectile : public Actor {
    public:
    glm::vec2 velocity;
    int damage;

    Projectile(const std::string& name, std::shared_ptr<PhysicsBody> body, glm::vec2 vel, int dmg) : Actor(name, body){velocity = vel; damage = dmg;}

void physics(float delta, const std::shared_ptr<World>& world) override {
  if (dead) return;
    // TODO MOVE ON PATH
    bool collided = false;
    *this += glm::vec2(velocity.x / 3* delta, 0);
    if (glm::length(velocity) && world->checkCollisions(world->staticBodies, *body)){
        collided = true;
    }

    *this += glm::vec2(0, velocity.y / 3* delta);
    if (glm::length(velocity) && world->checkCollisions(world->staticBodies, *body)){
        collided = true;
    }
    for (auto& x: world->enemies){
        if (!x->dead && x->getBody().isColliding(this->getBody())){
            x->decHp(damage);
            collided = true;
            break;
        }
    }
    if (collided) {
      dead = true;
    }
}
};
