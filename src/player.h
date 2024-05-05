#pragma once

#include "actor.h"
#include "body.h"
#include "inventory.h"
#include "attack_traits.h"
#include <memory>

#include <stdexcept>

#define MAX_WEAPONS 3
class Player final : public Actor, AttackTraits {
private:
    double scale = 0.5;
    GLuint texture;
    int curInvenSize = 0, maxInvenSize = MAX_WEAPONS;
    // stores pos
    
public:
    Weapon* curWeapon = nullptr;
    Inventory inventory; 
    bool prompt = false;
    Npc* promptGiver;
    glm::vec2 velocity{0,0};
    std::mutex velMuntx;

    mutable std::unordered_map<std::string, int> weaponNames;
    Player() = default;
    ~Player() = default;

    Player(glm::vec2 position, glm::vec2 size, auto& newTexture){
        body = std::shared_ptr<PhysicsBody>(new Rect(position, glm::vec2(size.x * scale, size.y * scale)));
        texture = newTexture;
        speed = 1;
        hp = 200;
    }

    std::string getWeapInd(int index){
        for (auto x: weaponNames){
            if (x.second == index)
                return x.first; 
        }
        return "";
    }
    int getIndex() const {
        if (!curWeapon) return -1;
        if (weaponNames.count(curWeapon->getName())){
            return weaponNames[curWeapon->getName()];
        }
        return -1;
    }
    void addWeapon(const std::string& str){
        if (curInvenSize > maxInvenSize - 1){
            throw std::runtime_error("Max Inventory Size Reached");
        }
        weaponNames.emplace(str, curInvenSize);
        curInvenSize++;
        inventory.addWeapon(str);
    }

    void removeWeapon(const std::string& str){
        weaponNames.erase(weaponNames.find(str));
        inventory.removeWeapon(str);
    }

    void physics(float delta, const std::shared_ptr<World>& world) override {
        *this += glm::vec2(velocity.x / 3 * delta, 0);
        if (glm::length(velocity) && world->checkCollisions(world->staticBodies, this->getRect())){
        	*this += glm::vec2(-1 * velocity.x / 3 * delta, 0);
        }

        *this += glm::vec2(0, velocity.y / 3 * delta);
        if (glm::length(velocity) && world->checkCollisions(world->staticBodies, this->getRect())){
        	*this += glm::vec2(0, -1 * velocity.y / 3 * delta);
        }
    }

    void render(ImDrawList* list, glm::vec2 Camera, float scale, float windowWidth, float windowHeight) override {
        list->AddImage((void*) texture, ImVec2((body->start().x - Camera.x) * scale + windowWidth / 2, (body->start().y - Camera.y) * scale + windowHeight / 2) , 
                                        ImVec2((body->end().x - Camera.x) * scale + windowWidth / 2, (body->end().y - Camera.y) * scale + windowHeight / 2) , ImVec2(0,0) , ImVec2(1, 1) , IM_COL32(255, 255, 255, 255));
    }

    void setVel(glm::vec2 vel){
        velMuntx.lock();
        velocity = vel;
        velMuntx.unlock();
    }

    rapidjson::Value toJSONObject() {
        rapidjson::Value value = Actor::toJSONObject();
        value["scale"].SetDouble(this->scale);
        value["texture"].SetUint(this->texture);
        return value;
    }

    void fromJSON(const rapidjson::Value& value) {
        Actor::fromJSON(value);
        this->scale = value["scale"].GetDouble();
        this->texture = value["texture"].GetUint();
    }
};
