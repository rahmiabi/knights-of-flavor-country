#pragma once

#include <string>
#include "item.h"
#include "csv_file.h"

enum class WeaponType {
    MELEE,
    GUN
};

class Weapon final : public Item {
private:
    uint32_t fireRate;
    uint32_t reloadSpeed;
    uint32_t ammo;
    uint32_t maxAmmo;
    uint32_t damage;
    uint32_t size;
    WeaponType type;
    uint32_t accuracy;
    uint32_t textureId = 0;

    static Weapon fromCsvEntry(const CsvEntry& entry);

    friend class ItemRegistry;

public:
    float time = 0;
    bool reloading = false;
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
    inline int getDamage() const {
        return this->damage;
    }

    inline int getMaxAmmo() const {
        return this->maxAmmo;
    }
    inline void setAmmo(int value) {
        this->ammo = value;
    }

    inline void incAmmo(int value = 1) {
        this->ammo += value;
    }

    inline void setReloading(bool val){
        reloading = val;
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
    
    inline void update(float delta){
        if (!reloading) return;

        time += delta;
        if (time > reloadSpeed){
            ammo = maxAmmo;
            time = 0;
            reloading = false;
        }

    }
};
