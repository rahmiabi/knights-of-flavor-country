#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>

#define RAPIDJSON_HAS_STDSTRING 1
#include "../lib/rapidjson/include/rapidjson/document.h"
#include "glm/vec2.hpp"

class Packet {
public:
    virtual ~Packet() = default;

    /**
     * @returns the packet's unique id
     */
    virtual uint8_t getId() const = 0;

    virtual std::string serialize() = 0;
};

/**
 * Packet client sends to server
 */
class PacketIn final : public Packet {
private:
    /**
     * key the user inputted
     */
    int key;
    /**
     * messages sent
     */
    std::vector<std::string> messages;

public:
    inline uint8_t getId() const {
        return 0x0;
    }

    inline int getKey() const {
        return this->key;
    }

    void setKey(int key) {
        this->key = key;
    }

    inline std::vector<std::string>& getMessages() {
        return this->messages;
    }

    inline const std::vector<std::string>& getMessages() const {
        return this->messages;
    }

    std::string serialize();
    void deserialize(const rapidjson::Value& value);
};

class PhysicsBody;
class Player;
class Enemy;
class Npc;

/**
 * Packet server sends to client
 */
class PacketOut final : public Packet {
private:
    std::unordered_map<std::string, glm::vec2> staticBodies; 
    std::unordered_map<std::string, glm::vec2> rigidBodies; 
    std::unordered_map<std::string, glm::vec2> players;
    std::unordered_map<std::string, glm::vec2> enemies;
    std::unordered_map<std::string, glm::vec2> npcs;
    std::vector<std::string> messages;

public:
    inline uint8_t getId() const {
        return 0x1;
    }

    inline std::unordered_map<std::string, glm::vec2>& getStaticBodies() { return this->staticBodies; }
    inline std::unordered_map<std::string, glm::vec2>& getRigidBodies() { return this->rigidBodies; } 
    inline std::unordered_map<std::string, glm::vec2>& getPlayers() { return this->players; }
    inline std::unordered_map<std::string, glm::vec2>& getEnemies() { return this->enemies; }
    inline std::unordered_map<std::string, glm::vec2>& getNpcs() { return this->npcs; }
    inline std::vector<std::string>& getMessages() { return this->messages; }

    std::string serialize();
    void deserialize(const rapidjson::Value& value);
};