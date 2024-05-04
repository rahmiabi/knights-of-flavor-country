/**
 * @purpose Npc class definitions and AI functionality
 * @author flakey5 (Aaron)
 */

#include "player.h"
#include "npc.h"


void Npc::physics(float delta, const std::shared_ptr<World>& world) {
    static float timer = 0;
    for (std::shared_ptr<Player>& player: world->players) {
        if (glm::length(player->getPos() - this->getPos()) < this->size().x * 4){
            if (!speaking)
                player->prompt = true;
            player->promptGiver = this;
        } 
    }
    {
        timer += delta;
        // show dialogue for 5 seconds
        if (timer >= 5000) {
            speaking = false;
            timer = 0;
        } 
    }
}
///////////////////////////////////////////////////////
//                    MCNpc
///////////////////////////////////////////////////////



///////////////////////////////////////////////////////
//                  MackerelliNpc
///////////////////////////////////////////////////////



///////////////////////////////////////////////////////
//                    RedmondNpc
///////////////////////////////////////////////////////



///////////////////////////////////////////////////////
//                    KerneyNpc
///////////////////////////////////////////////////////



///////////////////////////////////////////////////////
//                 CountVolodymyrNpc
///////////////////////////////////////////////////////


