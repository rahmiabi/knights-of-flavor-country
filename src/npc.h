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


    friend class ActorManager;

public:
    void update(float delta) override;
};

class MackerelliNpc final : public Npc {
private:
    MackerelliNpc(int64_t x, int64_t y);


    friend class ActorManager;

public:
    void update(float delta) override;
};

class RedmondNpc final : public Npc {
private:
    RedmondNpc(int64_t x, int64_t y);


    friend class ActorManager;

public:
    void update(float delta) override;
};

class KerneyNpc final : public Npc {
private:
    KerneyNpc(int64_t x, int64_t y);


    friend class ActorManager;

public:
    void update(float delta) override;
};

class CountVolodymyrNpc final : public Npc {
private:
    CountVolodymyrNpc(int64_t x, int64_t y);


    friend class ActorManager;

public:
    void update(float delta) override;
};
