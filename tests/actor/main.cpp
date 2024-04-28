#include "../../src/actor.h"
#include "../../src/enemy.h"

int main() {
    // printf("ActorManager::sActors.size() = %i\n", ActorManager::sActors.size());
    
    {
        auto m1 = ActorManager::create<M1ChipEnemy>(0, 0);
        printf("    m1->getName() = %s\n", m1->getName().c_str());
        printf("    m1.use_count() = %ld\n", m1.use_count());
    }

    // printf("ActorManager::sActors.size() = %i\n", ActorManager::sActors.size());
    
    auto ref = ActorManager::sActors["M1ChipEnemy"];
    // auto ref2 = ActorManager::sActors["M1ChipEnemy"];
    printf("use_count() = %ld\n", ref.use_count());

    ActorManager::remove("M1ChipEnemy");
    ActorManager::checkForDanglingActors();

    // printf("ActorManager::sActors.size() = %i\n", ActorManager::sActors.size());
    return 0;
}
