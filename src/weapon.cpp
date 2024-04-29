#include "weapon.h"

Weapon::Weapon(const std::string& name, int speed, WeaponType type, int accuracy) {
    this->name = name;
    this->speed = speed;
    this->type = type;
    this->accuracy = accuracy;
}
