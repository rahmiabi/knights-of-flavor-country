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
    uint32_t speed;
    uint32_t defense;
    uint32_t hp;

public:
    uint32_t getSpeed() const {
        return this->speed;
    }

    void setSpeed(uint32_t speed) {
        this->speed = speed;
    }
    
    uint32_t getDefense() const {
        return this->defense;
    }

    void setDefense(uint32_t defense) {
        this->defense = defense;
    }
    
    uint32_t getHp() const {
        return this->speed;
    }

    void setHp(uint32_t speed) {
        this->speed = speed;
    }
};
