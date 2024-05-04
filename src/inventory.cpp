
#include <stdexcept>
#include "inventory.h"

Ability& Inventory::getAbility(const std::string& name) {
    if (!this->abilities.contains(name)) {
        throw std::runtime_error("doesn't have ability");
    }
    return this->abilities.at(name);
}

const Ability& Inventory::getAbility(const std::string& name) const {
    if (!this->abilities.contains(name)) {
        throw std::runtime_error("doesn't have ability");
    }
    return this->abilities.at(name);
}

void Inventory::addAbility(const std::string& abilityName) {
    if (this->abilities.contains(abilityName)) {
        throw std::runtime_error("already has ability");
    }

//    const Ability& ability = ItemRegistry::getAbility(abilityName);
//    this->abilities.emplace(abilityName, ability);
}

void Inventory::removeAbility(const std::string& abilityName) {
    this->abilities.erase(abilityName);
}

Boost& Inventory::getBoost(const std::string& name) {
    if (!this->boosts.contains(name)) {
        throw std::runtime_error("doesn't have Boost");
    }
    return this->boosts.at(name);
}

const Boost& Inventory::getBoost(const std::string& name) const {
    if (!this->boosts.contains(name)) {
        throw std::runtime_error("doesn't have Boost");
    }
    return this->boosts.at(name);
}

void Inventory::addBoost(const std::string& boostName) {
    if (this->boosts.contains(boostName)) {
        throw std::runtime_error("already has boost");
    }

//    const Boost& boost = ItemRegistry::getBoost(boostName);
//    this->boosts.emplace(boostName, boost);
}

void Inventory::removeBoost(const std::string& boostName) {
    this->boosts.erase(boostName);
}

Weapon& Inventory::getWeapon(const std::string& name) {
    if (!this->weapons.contains(name)) {
        throw std::runtime_error("doesn't have Weapon");
    }
    return this->weapons.at(name);
}

const Weapon& Inventory::getWeapon(const std::string& name) const {
    if (!this->weapons.contains(name)) {
        throw std::runtime_error("doesn't have Weapon");
    }
    return this->weapons.at(name);
}

void Inventory::addWeapon(const std::string& weaponName) {
    if (this->weapons.contains(weaponName)) {
        throw std::runtime_error("already has weapon");
    }

    const Weapon& weapon = ItemRegistry::getWeapon(weaponName);
    this->weapons.emplace(weaponName, weapon);
}

void Inventory::removeWeapon(const std::string& weaponName) {
    this->weapons.erase(weaponName);
}
