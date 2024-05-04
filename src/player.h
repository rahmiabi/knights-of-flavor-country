#pragma once

#include "actor.h"
#include "body.h"
#include "inventory.h"
#include <glm/glm.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <GL/glew.h>
#include <GL/gl.h> // Will drag system OpenGL headers

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
    Weapon curWeapon;
    Inventory inventory; 
    mutable std::unordered_map<std::string, int> weaponNames;
    Player() = default;
    ~Player() = default;

    Player(glm::vec2 position, glm::vec2 size, auto& newTexture){
        body = std::shared_ptr<PhysicsBody>(new Rect(position, glm::vec2(size.x * scale, size.y * scale)));
        texture = newTexture;
    }


    glm::vec2 pos() const {
        return body->pos();
    }
    glm::vec2 size() const {
        return glm::vec2(body->size().x, body->size().y);
    }

    const PhysicsBody& getRect() const {
      	return *body.get();
    }
    std::string getWeapInd(int index){
        for (auto x: weaponNames){
            if (x.second == index)
                return x.first; 
        }
        return "";
    }
    int getIndex() const {
        if (weaponNames.count(curWeapon.getName())){
            return weaponNames[curWeapon.getName()];
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

    void render(ImDrawList* list, glm::vec2 Camera, float scale, float windowWidth, float windowHeight){
        list->AddImage((void*) texture, ImVec2((body->start().x - Camera.x) * scale + windowWidth / 2, (body->start().y - Camera.y) * scale + windowHeight / 2) , 
                                        ImVec2((body->end().x - Camera.x) * scale + windowWidth / 2, (body->end().y - Camera.y) * scale + windowHeight / 2) , ImVec2(0,0) , ImVec2(1, 1) , IM_COL32(255, 255, 255, 255));
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
