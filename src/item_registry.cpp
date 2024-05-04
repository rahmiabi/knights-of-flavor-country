#include "item_registry.h"

#include <unordered_map>
#include <string>
#include <stdexcept>
#include "csv_file.h"
#include <iostream>

static std::unordered_map<std::string, Ability> gAbilities;
static std::unordered_map<std::string, Boost> gBoosts;
static std::unordered_map<std::string, Weapon> gWeapons;

void ItemRegistry::init() {
//    CsvFile abilitiesFile("assets/abilities.csv");
//    if (abilitiesFile.hasParseError()) {
//        throw std::runtime_error("failed reading assets/abilities.csv, does it exist?");
//    }
//
//    for (auto& entry : abilitiesFile) {
//        Ability ability = Ability::fromCsvEntry(entry);
//        gAbilities.emplace(ability.getName(), ability);
//        // gAbilities[ability.getName()] = std::move(ability);
//    }
//
//    CsvFile boostsFile("assets/boosts.csv");
//    if (boostsFile.hasParseError()) {
//        throw std::runtime_error("failed reading assets/boosts.csv, does it exist?");
//    }
//
//    for (auto& entry : boostsFile) {
//        Boost boost = Boost::fromCsvEntry(entry);
//        gBoosts.emplace(boost.getName(), boost);
//    }

    CsvFile weaponsFile("assets/weapons.csv");
    if (weaponsFile.hasParseError()) {
        throw std::runtime_error("failed reading assets/weapons.csv, does it exist?");
    }

    for (auto& entry : weaponsFile) {
        Weapon weapon = Weapon::fromCsvEntry(entry);
        gWeapons.emplace(weapon.getName(), weapon);
        // gWeapons[weapon.getName()] = std::move(weapon);
    }
}

const Ability& ItemRegistry::getAbility(const std::string& name) {
    if (!gAbilities.contains(name)) {
        throw std::runtime_error("ItemRegistry::getAbility item doesn't exist");
    }

    return gAbilities.at(name);
}

const Boost& ItemRegistry::getBoost(const std::string& name) {
    if (!gAbilities.contains(name)) {
        throw std::runtime_error("ItemRegistry::getBoost item doesn't exist");
    }

    return gBoosts.at(name);
}

const Weapon& ItemRegistry::getWeapon(const std::string& name) {
    if (!gWeapons.contains(name)) {
        throw std::runtime_error("ItemRegistry::getWeapon item doesn't exist");
    }

    //std::cout << name << std::endl;
    return gWeapons.at(name);
}
