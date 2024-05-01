/**
 * @purpose Enemy class definitions
 * @author flakey5 (Aaron)
 */

#pragma once

#include "npc.h"
#include "attack_traits.h"

/**
 * Base class for all enemies
 */
class Enemy : public Npc, AttackTraits {
protected:
    Enemy(const std::string& name, int64_t x, int64_t y);

public:
    virtual ~Enemy() = default;
};

class M1ChipEnemy final : public Enemy {
private:
    M1ChipEnemy(int64_t x, int64_t y);

    friend class ActorManager;

public:
    void update(float delta) override;
};

class M2ChipEnemy final : public Enemy {
private:
    M2ChipEnemy(int64_t x, int64_t y);


    friend class ActorManager;

public:
    void update(float delta) override;
};

class M3ChipEnemy final : public Enemy {
private:
    M3ChipEnemy(int64_t x, int64_t y);


    friend class ActorManager;

public:
    void update(float delta) override;
};

class M4ChipEnemy final : public Enemy {
private:
    M4ChipEnemy(int64_t x, int64_t y);


    friend class ActorManager;

public:
    void update(float delta) override;
};

class EvilSurinEnemy final : public Enemy {
private:
    EvilSurinEnemy(int64_t x, int64_t y);


    friend class ActorManager;

public:
    void update(float delta) override;
};

class EvilKerneyEnemy final : public Enemy {
private:
    EvilKerneyEnemy(int64_t x, int64_t y);


    friend class ActorManager;

public:
    void update(float delta) override;
};
