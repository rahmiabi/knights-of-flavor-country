/**
 * @purpose Enemy class definitions
 * @author flakey5 (Aaron)
 */

#pragma once

#include "player.h"
#include "npc.h"
#include "attack_traits.h"
#include <queue>
#include <unordered_set>
#include <cmath>

/**
 * Base class for all enemies
 */
class Enemy : public Npc, AttackTraits {
protected:
    std::deque<glm::vec2> path;
    //how long in ms should enemy pathfind
    float pathRefresh;
public:
    float delta = 0;
    Enemy(const std::string& name, std::shared_ptr<PhysicsBody> body) : Npc(name, body){}
    virtual ~Enemy() = default;

    void update(const std::shared_ptr<World> world);
    std::deque<glm::vec2> aStar(const std::vector<std::shared_ptr<PhysicsBody>>& space, const glm::vec2& pos, const glm::vec2& finalPos, const float& step, const glm::vec2& size);
};

class M1ChipEnemy final : public Enemy {
private:

public:
    M1ChipEnemy(const std::string& name, std::shared_ptr<PhysicsBody> body) : Enemy(name, body){}
    void update(float delta) override;
};

class M2ChipEnemy final : public Enemy {
private:

    friend class ActorManager;

public:
    M2ChipEnemy(const std::string& name, std::shared_ptr<PhysicsBody> body) : Enemy(name, body){}
    void update(float delta) override;
};

class M3ChipEnemy final : public Enemy {
private:
    friend class ActorManager;

public:
    M3ChipEnemy(const std::string& name, std::shared_ptr<PhysicsBody> body) : Enemy(name, body){}
    void update(float delta) override;
};

class M4ChipEnemy final : public Enemy {
private:

public:
    M4ChipEnemy(const std::string& name, std::shared_ptr<PhysicsBody> body) : Enemy(name, body){}
    void update(float delta) override;
};

class EvilSurinEnemy final : public Enemy {
private:


public:
    EvilSurinEnemy(const std::string& name, std::shared_ptr<PhysicsBody> body) : Enemy(name, body){}
    void update(float delta) override;
};

class EvilKerneyEnemy final : public Enemy {
private:

public:
    EvilKerneyEnemy(const std::string& name, std::shared_ptr<PhysicsBody> body) : Enemy(name, body){}
    void update(float delta) override;
};
