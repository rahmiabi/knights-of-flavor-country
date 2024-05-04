/**
 * @purpose Inventory class definitions
 * @author flakey5 (Aaron)
 */

#pragma once
#include "item_registry.h"

#include <cstdint>
#include <map>
#include <unordered_map>
#include "item.h"
#include "ability.h"
#include "boost.h"
#include "weapon.h"

class Inventory {
private:
    std::unordered_map<std::string, Ability> abilities;
    std::unordered_map<std::string, Boost> boosts;
    std::unordered_map<std::string, Weapon> weapons;

public:
    inline bool hasAbility(const std::string& name) {
        return this->abilities.contains(name);
    }

    Ability& getAbility(const std::string& name);
    const Ability& getAbility(const std::string& name) const;

    void addAbility(const std::string& name);
    void removeAbility(const std::string& name);

    inline bool hasBoost(const std::string& name) {
        return this->boosts.contains(name);
    }

    Boost& getBoost(const std::string& name);
    const Boost& getBoost(const std::string& name) const;

    void addBoost(const std::string& name);
    void removeBoost(const std::string& name);

    inline bool hasWeapon(const std::string& name) {
        return this->weapons.contains(name);
    }

    Weapon& getWeapon(const std::string& name);
    const Weapon& getWeapon(const std::string& name) const;

    void addWeapon(const std::string& name);
    void removeWeapon(const std::string& name);
};
