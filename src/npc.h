/**
 * @purpose Npc class definitions
 * @author flakey5 (Aaron)
 */

#pragma once

#include "actor.h"

class Npc : public Actor {
protected:
    Npc(const std::string& name, int64_t x, int64_t y);

public:
    virtual ~Npc() = default;
};

class MCNpc final : public Npc {
private:
    MCNpc(int64_t x, int64_t y);

    void update(float delta) override;

    friend class ActorManager;

public:
};

class MackerelliNpc final : public Npc {
private:
    MackerelliNpc(int64_t x, int64_t y);

    void update(float delta) override;

    friend class ActorManager;

public:
};

class RedmondNpc final : public Npc {
private:
    RedmondNpc(int64_t x, int64_t y);

    void update(float delta) override;

    friend class ActorManager;

public:
};

class KerneyNpc final : public Npc {
private:
    KerneyNpc(int64_t x, int64_t y);

    void update(float delta) override;

    friend class ActorManager;

public:
};

class CountVolodymyrNpc final : public Npc {
private:
    CountVolodymyrNpc(int64_t x, int64_t y);

    void update(float delta) override;

    friend class ActorManager;

public:
};
