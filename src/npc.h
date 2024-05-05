/**
 * @purpose Npc class definitions
 * @author flakey5 (Aaron)
 */

#pragma once

#include "actor.h"
#include <deque>

class Npc : public Actor {

public:
    std::deque<std::string> planned;
    std::vector<std::string> dialogue;

    bool speaking = false;
    Npc() = default;
    Npc(std::string name, std::shared_ptr<PhysicsBody> body) : Actor(name, body){}
    virtual ~Npc() = default;

    void physics(float delta, const std::shared_ptr<World>& world) override;
};

class MCNpc final : public Npc {
private:

public:
    MCNpc() = default;
    MCNpc(std::string name, std::shared_ptr<PhysicsBody> body) : Npc(name, body){}
};

class MackerelliNpc final : public Npc {
private:

public:
    MackerelliNpc() = default;
    MackerelliNpc(std::string name, std::shared_ptr<PhysicsBody> body) : Npc(name, body){}
};

class RedmondNpc final : public Npc {
private:
public:
    RedmondNpc() = default;
    RedmondNpc(std::string name, std::shared_ptr<PhysicsBody> body) : Npc(name, body){}
};

class KerneyNpc final : public Npc {
private:

public:
    KerneyNpc() = default;
    KerneyNpc(std::string name, std::shared_ptr<PhysicsBody> body) : Npc(name, body){}
};

class CountVolodymyrNpc final : public Npc {
private:
public:
    CountVolodymyrNpc() = default;
    CountVolodymyrNpc(std::string name, std::shared_ptr<PhysicsBody> body) : Npc(name, body){}
};
