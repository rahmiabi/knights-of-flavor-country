/**
 * @purpose Enemy class implementation
 * @author flakey5 (Aaron)
 */

#include "actor.h"

#include "player.h"

Actor::Actor(const std::string name, std::shared_ptr<PhysicsBody> body) {
    this->name = name;
    this->body = body;
}

void Actor::update(float delta, const std::shared_ptr<World>& world ){
    return;
}
void Actor::physics(float delta, const std::shared_ptr<World>& world ){
    return;
}

std::string Actor::toJSON(rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>& allocator) {
    rapidjson::Value json = this->toJSONObject(allocator);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    json.Accept(writer);

    return buffer.GetString();
}

rapidjson::Value Actor::toJSONObject(rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>& allocator) {
    rapidjson::Value json;
    json.SetObject();
    json.AddMember(rapidjson::Value("name"), rapidjson::Value(this->name.c_str(), this->name.size()), allocator);
    
    rapidjson::Value pos;
    pos.SetObject();
    pos.AddMember("x", this->body->pos().x, allocator);
    pos.AddMember("y", this->body->pos().y, allocator);
    json.AddMember("pos", pos, allocator);

    json.AddMember("physicsBody", this->body->toJSONObject(allocator), allocator);

    return json;
}

void Actor::fromJSON(const rapidjson::Value& value) {
    if (!value.HasMember("name") || !value.HasMember("pos")) {
        throw std::runtime_error("Actor::fromJSON was passed incorrect json");
    }

    this->name = value["name"].GetString();    
    this->body->fromJSONObject(value["physicsBody"].GetObj());
}



void World::checkForDanglingActors() {
    for (auto& [key, value] : this->actors) {
        if (value.use_count() == 1) {
            printf("FOUND DANGLING ACTOR: %s\n", key.c_str());
        }
    }
}

std::string World::toJSON() {
    rapidjson::Document json;
    json.SetObject();

    rapidjson::Value actors;
    actors.SetArray();
    for (const auto& [_, actor] : this->actors) {
        rapidjson::Value value = actor->toJSONObject(json.GetAllocator());
        actors.PushBack(value, json.GetAllocator());
    }
    json.AddMember("actors", actors, json.GetAllocator());

    rapidjson::Value staticBodies;
    staticBodies.SetArray();
    for (const auto& body : this->staticBodies) {
        rapidjson::Value value = body->toJSONObject(json.GetAllocator());
        staticBodies.PushBack(value, json.GetAllocator());
    }
    json.AddMember("staticBodies", staticBodies, json.GetAllocator());

    rapidjson::Value rigidBodies;
    rigidBodies.SetArray();
    for (const auto& body : this->rigidBodies) {
        rapidjson::Value value = body->toJSONObject(json.GetAllocator());
        rigidBodies.PushBack(value, json.GetAllocator());
    }
    json.AddMember("rigidBodies", rigidBodies, json.GetAllocator());

    rapidjson::Value players;
    players.SetArray();
    for (const auto& body : this->players) {
        rapidjson::Value value = body->toJSONObject();
        players.PushBack(value, json.GetAllocator());
    }
    json.AddMember("players", players, json.GetAllocator());

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    json.Accept(writer);

    return buffer.GetString();
}


void World::fromJSON(const rapidjson::Value& json) {
    const rapidjson::Value& actors = json["actors"].GetArray();
    for (rapidjson::Value::ConstValueIterator iter = actors.Begin(); iter != actors.End(); iter++) {
        //auto actor = std::make_shared<Actor>();
        //actor->fromJSON(*iter);
        //this->actors[actor->getName()] = actor;
    }
    
    const rapidjson::Value& staticBodies = json["staticBodies"].GetArray();
    for (rapidjson::Value::ConstValueIterator iter = staticBodies.Begin(); iter != staticBodies.End(); iter++) {
        //auto actor = std::make_shared<PhysicsBody>();
        //actor->fromJSONObject(*iter);
        //this->staticBodies.push_back(actor);
    }
    
    const rapidjson::Value& rigidBodies = json["rigidBodies"].GetArray();
    for (rapidjson::Value::ConstValueIterator iter = rigidBodies.Begin(); iter != staticBodies.End(); iter++) {
        //auto actor = std::make_shared<PhysicsBody>();
        //actor->fromJSONObject(*iter);
        //this->rigidBodies.push_back(actor);
    }
    
    const rapidjson::Value& players = json["players"].GetArray();
    for (rapidjson::Value::ConstValueIterator iter = players.Begin(); iter != staticBodies.End(); iter++) {
        //auto actor = std::make_shared<Player>();
        //actor->fromJSON(*iter);
        //this->players.push_back(actor);
    }
}
