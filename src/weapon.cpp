#include "weapon.h"

#include <stdexcept>

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
    ASSERT_COLUMN(entry, "textureId");

    Weapon weapon;
    weapon.name = entry["name"];
    weapon.description = entry["description"];
    weapon.ammo = entry.getU32("ammo");
    weapon.size = entry.getU32("size");
    
    const std::string& weaponType = entry.getValueAtColumn("type");
    if (!stringToWeaponTypeMap.contains(weaponType)) {
        throw std::runtime_error("invalid weapon type");
    }
    weapon.type = stringToWeaponTypeMap[weaponType];

    weapon.accuracy = entry.getU32("accuracy");
    weapon.textureId = entry.getU32("textureID");
}
