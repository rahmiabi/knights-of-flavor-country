#pragma once

#include "item.h"
#include "ability.h"
#include "boost.h"
#include "weapon.h"

class ItemRegistry {
public:
    ItemRegistry() = default;
    static void init();

    static const Ability& getAbility(const std::string& name);
    static const Boost& getBoost(const std::string& name);
    static const Weapon& getWeapon(const std::string& name);
};
