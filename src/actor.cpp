/**
 * @purpose Enemy class implementation
 * @author flakey5 (Aaron)
 */

#include "actor.h"
#include <cstdio>

std::unordered_map<std::string, std::shared_ptr<Actor>> ActorManager::sActors;

Actor::Actor(const std::string name, int64_t x, int64_t y) {
    this->name = name;
    this->x = x;
    this->y = y;
}

void ActorManager::remove(const std::string& name) {
    if (!sActors.contains(name)) {
        printf(
            "ActorManager::remove: tried removing non-existent actor %s, ignoring\n",
            name.c_str()
        );
        return;
    }
    
    auto& ptr = sActors[name]; // ref so we don't increase ref count on ptr
    if (ptr.use_count() > 2) {
        printf(
            "ActorManager::remove: actor %s has >2 refs (actual=%ld), possible memory leak\n",
            name.c_str(),
            ptr.use_count()
        );
    }

    sActors.erase(name);
}

void ActorManager::checkForDanglingActors() {
    for (auto& [key, value] : sActors) {
        if (value.use_count() == 1) {
            printf("FOUND DANGLING ACTOR: %s\n", key.c_str());
        }
    }
}
