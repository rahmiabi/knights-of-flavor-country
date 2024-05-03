/**
 * @purpose Enemy class implementation
 * @author flakey5 (Aaron)
 */

#include "actor.h"

#include <cstdio>
#include "../lib/rapidjson/include/rapidjson/document.h"
#include "../lib/rapidjson/include/rapidjson/writer.h"
#include "../lib/rapidjson/include/rapidjson/stringbuffer.h"

Actor::Actor(const std::string name, std::shared_ptr<PhysicsBody> body) {
    this->name = name;
    this->body = body;
}

void Actor::update(float delta, const std::shared_ptr<World>& world ){

}
void Actor::physics(float delta, const std::shared_ptr<World>& world ){

}
std::string Actor::toJSON() {
    rapidjson::Document json;
    json["name"].SetString(this->name.c_str(), this->name.size());
    
    rapidjson::Value& pos = json["pos"].SetObject();
    pos["x"].SetFloat(this->body->pos().x);
    pos["y"].SetFloat(this->body->pos().y);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    json.Accept(writer);

    return buffer.GetString();
}

void Actor::fromJSON(const rapidjson::Value& value) {
    if (!value.HasMember("name") || !value.HasMember("pos")) {
        throw std::runtime_error("Actor::fromJSON was passed incorrect json");
    }

    this->name = value["name"].GetString();
    
    const rapidjson::Value& pos = value["pos"].GetObj();
    this->setPosition({pos["x"].GetFloat(), pos["y"].GetFloat()});
}

void World::removeActor(const std::string& name) {
    if (!this->actors.contains(name)) {
        printf(
            "ActorManager::remove: tried removing non-existent actor %s, ignoring\n",
            name.c_str()
        );
        return;
    }
    
    auto& ptr = this->actors[name]; // ref so we don't increase ref count on ptr
    if (ptr.use_count() > 2) {
        printf(
            "ActorManager::remove: actor %s has >2 refs (actual=%ld), possible memory leak\n",
            name.c_str(),
            ptr.use_count()
        );
    }

    this->actors.erase(name);
}

void World::checkForDanglingActors() {
    for (auto& [key, value] : this->actors) {
        if (value.use_count() == 1) {
            printf("FOUND DANGLING ACTOR: %s\n", key.c_str());
        }
    }
}
