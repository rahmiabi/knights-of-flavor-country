/**
 * @purpose Shared traits between the player and Npcs
 * @author flakey5 (Aaron)
 */

#pragma once

#include <cstdint>

/**
 * Shared traits between the player and npcs for  
 */
class AttackTraits {
protected:
    int32_t speed;
    int32_t defense;
    int32_t hp;

public:
    AttackTraits() = default;
    int32_t getSpeed() const {
        return this->speed;
    }

    void setSpeed(int32_t speed) {
        this->speed = speed;
    }
    
    int32_t getDefense() const {
        return this->defense;
    }

    void setDefense(int32_t defense) {
        this->defense = defense;
    }
    
    int32_t getHp() const {
        return this->speed;
    }

    void setHp(int32_t speed) {
        this->speed = speed;
    }
};
