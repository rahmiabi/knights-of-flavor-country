#pragma once

#include <string>

enum class WeaponType {
    MELEE,
    GUN
};

class Weapon {
private:
    std::string name;
    int speed;
    WeaponType type;
    int accuracy;

public:
    Weapon(const std::string& name, int speed, WeaponType type, int accuracy);

    inline const std::string& getName() const {
        return this->name;
    }

    inline void setName(const std::string& name) {
        this->name = name;
    }

    inline int getSpeed() const {
        return this->speed;
    }

    inline void setSpeed(int speed) {
        this->speed = speed;
    }

    inline WeaponType getType() const {
        return this->type;
    }

    inline void setType(WeaponType type) {
        this->type = type;
    }

    inline int getAccuracy() const {
        return this->accuracy;
    }

    inline void setAccuracy(int accuracy) {
        this->accuracy = accuracy;
    }
};
