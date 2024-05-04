#pragma once

#include <string>
#include "item.h"
#include "csv_file.h"

enum class WeaponType {
    MELEE,
    GUN
};

class ItemRegistry;
class Weapon final : public Item {
private:
    uint32_t fireRate;
    uint32_t reloadSpeed;
    uint32_t ammo;
    uint32_t size;
    WeaponType type;
    uint32_t accuracy;
    uint32_t textureId;


    static Weapon fromCsvEntry(const CsvEntry& entry);

    friend class ItemRegistry;

public:
    Weapon() = default;
    Weapon(const Weapon&) = default;

    // Character can only have one of a weapon
    inline uint32_t getMaxAmount() override {
        return 1;
    }

    inline int getFireRate() const {
        return this->fireRate;
    }

    inline int getReloadSpeed() const {
        return this->reloadSpeed;
    }

    inline int getAmmo() const {
        return this->ammo;
    }

    inline void setAmmo(int value) {
        this->ammo = value;
    }

    inline void incAmmo(int value = 1) {
        this->ammo += value;
    }

    inline void decAmmo(int value = 1) {
        this->ammo -= value;
    }

    inline int getSize() const {
        return this->size;
    }

    inline WeaponType getType() const {
        return this->type;
    }

    inline int getAccuracy() const {
        return this->accuracy;
    }

    inline int getTextureId() const {
        return this->textureId;
    }
};
