/**
 * @purpose Enemy class definitions and AI functionality
 * @author flakey5 (Aaron)
 */

#include "enemy.h"

Enemy::Enemy(const std::string& name, int64_t x, int64_t y) : Npc(name, x, y) {
}

///////////////////////////////////////////////////////
//                    M1ChipEnemy
///////////////////////////////////////////////////////

M1ChipEnemy::M1ChipEnemy(int64_t x, int64_t y) : Enemy("M1ChipEnemy", x, y) {
}

void M1ChipEnemy::update(float delta) {
    // TODO
}

///////////////////////////////////////////////////////
//                    M2ChipEnemy
///////////////////////////////////////////////////////

M2ChipEnemy::M2ChipEnemy(int64_t x, int64_t y) : Enemy("M2ChipEnemy", x, y) {
}

void M2ChipEnemy::update(float delta) {
    // TODO
}

///////////////////////////////////////////////////////
//                    M3ChipEnemy
///////////////////////////////////////////////////////

M3ChipEnemy::M3ChipEnemy(int64_t x, int64_t y) : Enemy("M3ChipEnemy", x, y) {
}

void M3ChipEnemy::update(float delta) {
    // TODO
}

///////////////////////////////////////////////////////
//                    M4ChipEnemy
///////////////////////////////////////////////////////

M4ChipEnemy::M4ChipEnemy(int64_t x, int64_t y) : Enemy("M4ChipEnemy", x, y) {
}

void M4ChipEnemy::update(float delta) {
    // TODO
}

///////////////////////////////////////////////////////
//                  EvilSurinEnemy
///////////////////////////////////////////////////////

EvilSurinEnemy::EvilSurinEnemy(int64_t x, int64_t y) : Enemy("EvilSurinEnemy", x, y) {
}

void EvilSurinEnemy::update(float delta) {
    // TODO
}

///////////////////////////////////////////////////////
//                 EvilKerneyEnemy
///////////////////////////////////////////////////////

EvilKerneyEnemy::EvilKerneyEnemy(int64_t x, int64_t y) : Enemy("EvilKerneyEnemy", x, y) {
}

void EvilKerneyEnemy::update(float delta) {
    // TODO
}
