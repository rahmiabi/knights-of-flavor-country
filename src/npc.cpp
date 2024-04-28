/**
 * @purpose Npc class definitions and AI functionality
 * @author flakey5 (Aaron)
 */

#include "npc.h"

Npc::Npc(const std::string& name, int64_t x, int64_t y) : Actor(name, x, y) {
}

///////////////////////////////////////////////////////
//                    MCNpc
///////////////////////////////////////////////////////

MCNpc::MCNpc(int64_t x, int64_t y) : Npc("MCNpc", x, y) {
}

void MCNpc::update(float delta) {
    // TODO
}

///////////////////////////////////////////////////////
//                  MackerelliNpc
///////////////////////////////////////////////////////

MackerelliNpc::MackerelliNpc(int64_t x, int64_t y) : Npc("MackerelliNpc", x, y) {
}

void MackerelliNpc::update(float delta) {
    // TODO
}

///////////////////////////////////////////////////////
//                    RedmondNpc
///////////////////////////////////////////////////////

RedmondNpc::RedmondNpc(int64_t x, int64_t y) : Npc("RedmondNpc", x, y) {
}

void RedmondNpc::update(float delta) {
    // TODO
}

///////////////////////////////////////////////////////
//                    KerneyNpc
///////////////////////////////////////////////////////

KerneyNpc::KerneyNpc(int64_t x, int64_t y) : Npc("KerneyNpc", x, y) {
}

void KerneyNpc::update(float delta) {
    // TODO
}

///////////////////////////////////////////////////////
//                 CountVolodymyrNpc
///////////////////////////////////////////////////////

CountVolodymyrNpc::CountVolodymyrNpc(int64_t x, int64_t y) : Npc("CountVolodymyrNpc", x, y) {
}

void CountVolodymyrNpc::update(float delta) {
    // TODO
}
