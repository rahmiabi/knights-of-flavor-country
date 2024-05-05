/**
 * @purpose Npc class definitions and AI functionality
 * @author flakey5 (Aaron)
 */

#include "player.h"
#include "npc.h"


void Npc::physics(float delta, const std::shared_ptr<World>& world) {
    for (std::shared_ptr<Player>& player: world->players) {
        if (glm::length(player->getPos() - this->getPos()) < this->size().x * 4){
            if (!speaking)
                player->prompt = true;
            player->promptGiver = this;
        } 
    }
    if (!speaking) return;
    static float timer = 0;
    {
        timer += delta;
        // show dialogue for 2.5 seconds
        if (timer >= 3000) {
            if (planned.empty())
                speaking = false;
            else{
                planned.pop_front(); 
                if (planned.empty()) speaking = false;
            }
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


