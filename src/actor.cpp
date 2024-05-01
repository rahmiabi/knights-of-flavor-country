/**
 * @purpose Enemy class implementation
 * @author flakey5 (Aaron)
 */

#include "actor.h"
#include <cstdio>

Actor::Actor(const std::string name, std::shared_ptr<PhysicsBody> body) {
    this->name = name;
    this->body = body;
}

void World::removeActor(const std::string& name) {
    if (!this->actors.contains(name)) {
        printf(
            "ActorManager::remove: tried removing non-existent actor %s, ignoring\n",
            name.c_str()
        );
        return;
    }
    
    auto& ptr = this->actors[name]; // ref so we don't increase ref count on ptr
    if (ptr.use_count() > 2) {
        printf(
            "ActorManager::remove: actor %s has >2 refs (actual=%ld), possible memory leak\n",
            name.c_str(),
            ptr.use_count()
        );
    }

    this->actors.erase(name);
}

void World::checkForDanglingActors() {
    for (auto& [key, value] : this->actors) {
        if (value.use_count() == 1) {
            printf("FOUND DANGLING ACTOR: %s\n", key.c_str());
        }
    }
}
