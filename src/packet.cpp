#include "packet.h"

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "actor.h"
#include "player.h"
#include "enemy.h"

std::string PacketIn::serialize() {
    rapidjson::Document json;
    json.SetObject();

    json.AddMember("key", this->key, json.GetAllocator());

    rapidjson::Value messagesJson;
    messagesJson.SetArray();
    for (auto& message : this->messages) {
        messagesJson.PushBack(rapidjson::Value(message.c_str(), message.size()), json.GetAllocator());
    }
    json.AddMember("messages", messagesJson, json.GetAllocator());

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    json.Accept(writer);

    return buffer.GetString();
}

void PacketIn::deserialize(const rapidjson::Value& json) {
    this->key = json["key"].GetInt();

    const auto& messagesJson = json["messages"].GetArray();
    this->messages.resize(messagesJson.Size());

    size_t i = 0;
    for (rapidjson::Value::ConstValueIterator iter = messagesJson.Begin(); iter != messagesJson.End(); iter++) {
        this->messages[i] = iter->GetString();
        i++;
    }
}

static void serializeVec2(rapidjson::Value& parent, glm::vec2 pos) {
    parent["x"].SetFloat(pos.x);
    parent["y"].SetFloat(pos.y);
}

std::string PacketOut::serialize() {
    rapidjson::Document json;

    rapidjson::Value players;
    players.SetArray();
    for (const auto& [name, pos] : this->players) {
        rapidjson::Value player;
        player.SetObject();
        player["name"].SetString(name.c_str(), name.size());
        auto& posJson = player["pos"].SetObject();
        serializeVec2(posJson, pos);
        players.PushBack(player, json.GetAllocator());
    }
    json.AddMember("players", players, json.GetAllocator());

    rapidjson::Value enemies;
    enemies.SetArray();
    for (const auto& [name, pos] : this->enemies) {
        rapidjson::Value player;
        player.SetObject();
        player["name"].SetString(name.c_str(), name.size());
        auto& posJson = player["pos"].SetObject();
        serializeVec2(posJson, pos);
        players.PushBack(player, json.GetAllocator());
    }
    json.AddMember("enemies", enemies, json.GetAllocator());

    rapidjson::Value npcs;
    npcs.SetArray();
    for (const auto& [name, pos] : this->npcs) {
        rapidjson::Value player;
        player.SetObject();
        player["name"].SetString(name.c_str(), name.size());
        auto& posJson = player["pos"].SetObject();
        serializeVec2(posJson, pos);
        players.PushBack(player, json.GetAllocator());
    }
    json.AddMember("npcs", npcs, json.GetAllocator());

    rapidjson::Value staticBodies;
    staticBodies.SetArray();
    for (const auto& [name, pos] : this->staticBodies) {
        rapidjson::Value player;
        player.SetObject();
        player["name"].SetString(name.c_str(), name.size());
        auto& posJson = player["pos"].SetObject();
        serializeVec2(posJson, pos);
        staticBodies.PushBack(player, json.GetAllocator());
    }
    json.AddMember("staticBodies", staticBodies, json.GetAllocator());

    rapidjson::Value rigidBodies;
    rigidBodies.SetArray();
    for (const auto& [name, pos] : this->rigidBodies) {
        rapidjson::Value player;
        player.SetObject();
        player["name"].SetString(name.c_str(), name.size());
        auto& posJson = player["pos"].SetObject();
        serializeVec2(posJson, pos);
        rigidBodies.PushBack(player, json.GetAllocator());
    }
    json.AddMember("rigidBodies", rigidBodies, json.GetAllocator());

    rapidjson::Value messagesJson;
    messagesJson.SetArray();
    for (auto& message : this->messages) {
        messagesJson.PushBack(rapidjson::Value(message.c_str(), message.size()), json.GetAllocator());
    }
    json.AddMember("messages", messagesJson, json.GetAllocator());

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    json.Accept(writer);

    return buffer.GetString();
}

static glm::vec2 deserializeVec2(const rapidjson::Value& json) {
    return glm::vec2 { json["x"].GetFloat(), json["y"].GetFloat() };
}

void PacketOut::deserialize(const rapidjson::Value& json) {
    const auto& players = json["players"].GetArray();
    for (rapidjson::Value::ConstValueIterator iter = players.Begin(); iter != players.End(); iter++) {
        const char* name = iter->GetObj()["name"].GetString();
        const auto& pos = iter->GetObject()["pos"].GetObj();
        this->players[name] = deserializeVec2(pos);
    }

    const auto& enemies = json["enemies"].GetArray();
    for (rapidjson::Value::ConstValueIterator iter = enemies.Begin(); iter != enemies.End(); iter++) {
        const char* name = iter->GetObj()["name"].GetString();
        const auto& pos = iter->GetObject()["pos"].GetObj();
        this->enemies[name] = deserializeVec2(pos);
    }

    const auto& npcs = json["npcs"].GetArray();
    for (rapidjson::Value::ConstValueIterator iter = npcs.Begin(); iter != npcs.End(); iter++) {
        const char* name = iter->GetObj()["name"].GetString();
        const auto& pos = iter->GetObject()["pos"].GetObj();
        this->npcs[name] = deserializeVec2(pos);
    }
    
    const auto& staticBodies = json["staticBodies"].GetArray();
    for (rapidjson::Value::ConstValueIterator iter = staticBodies.Begin(); iter != staticBodies.End(); iter++) {
        const char* name = iter->GetObj()["name"].GetString();
        const auto& pos = iter->GetObject()["pos"].GetObj();
        this->staticBodies[name] = deserializeVec2(pos);
    }
    
    const auto& rigidBodies = json["rigidBodies"].GetArray();
    for (rapidjson::Value::ConstValueIterator iter = rigidBodies.Begin(); iter != rigidBodies.End(); iter++) {
        const char* name = iter->GetObj()["name"].GetString();
        const auto& pos = iter->GetObject()["pos"].GetObj();
        this->rigidBodies[name] = deserializeVec2(pos);
    }

    int i = 0;
    const auto& messagesJson = json["messages"].GetArray();
    this->messages.resize(messagesJson.Size());
    for (rapidjson::Value::ConstValueIterator iter = messagesJson.Begin(); iter != messagesJson.End(); iter++) {
        this->messages[i] = iter->GetString();
        i++;
    }
}
