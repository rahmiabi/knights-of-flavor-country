/**
 * @purpose Enemy class definitions
 * @author flakey5 (Aaron)
 */

#pragma once

#include "npc.h"

/**
 * Base class for all enemies
 */
class Enemy : public Npc {
protected:
    Enemy(const std::string& name, int64_t x, int64_t y);

public:
    virtual ~Enemy() = default;
};

class M1ChipEnemy final : public Enemy {
private:
    M1ChipEnemy(int64_t x, int64_t y);

    void update(float delta) override;

    friend class ActorManager;

public:
};

class M2ChipEnemy final : public Enemy {
private:
    M2ChipEnemy(int64_t x, int64_t y);

    void update(float delta) override;

    friend class ActorManager;

public:
};

class M3ChipEnemy final : public Enemy {
private:
    M3ChipEnemy(int64_t x, int64_t y);

    void update(float delta) override;

    friend class ActorManager;

public:
};

class M4ChipEnemy final : public Enemy {
private:
    M4ChipEnemy(int64_t x, int64_t y);

    void update(float delta) override;

    friend class ActorManager;

public:
};

class EvilSurinEnemy final : public Enemy {
private:
    EvilSurinEnemy(int64_t x, int64_t y);

    void update(float delta) override;

    friend class ActorManager;

public:
};

class EvilKerneyEnemy final : public Enemy {
private:
    EvilKerneyEnemy(int64_t x, int64_t y);

    void update(float delta) override;

    friend class ActorManager;

public:
};
