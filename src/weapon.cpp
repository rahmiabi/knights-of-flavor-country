#include "weapon.h"

#include <stdexcept>
#include <iostream>
static std::unordered_map<std::string, WeaponType> stringToWeaponTypeMap = {
    { "MELEE", WeaponType::MELEE },
    { "GUN", WeaponType::GUN }
};

Weapon Weapon::fromCsvEntry(const CsvEntry& entry) {
    ASSERT_COLUMN(entry, "name");
    ASSERT_COLUMN(entry, "description");
    ASSERT_COLUMN(entry, "fireRate");
    ASSERT_COLUMN(entry, "reloadSpeed");
    ASSERT_COLUMN(entry, "ammo");
    ASSERT_COLUMN(entry, "size");
    ASSERT_COLUMN(entry, "type");
    ASSERT_COLUMN(entry, "accuracy");
    ASSERT_COLUMN(entry, "textureID");

    Weapon weapon;
    weapon.name = entry["name"];
    weapon.description = entry["description"];
    weapon.ammo = entry.getU32("ammo");
    weapon.reloadSpeed = entry.getU32("reloadSpeed");
    weapon.fireRate = entry.getU32("fireRate");
    weapon.size = entry.getU32("size");
    
    const std::string& weaponType = entry["type"];
    if (!stringToWeaponTypeMap.contains(weaponType)) {
        throw std::runtime_error("invalid weapon type");
    }
    weapon.type = stringToWeaponTypeMap[weaponType];

    weapon.accuracy = entry.getI32("accuracy");
    weapon.textureId = entry.getI32("textureID");
    return weapon;
}
