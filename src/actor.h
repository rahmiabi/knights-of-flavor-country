/**
 * @purpose Actor class definition
 * @author flakey5 (Aaron)
 */

#pragma once

#include <cstdint>
#include <string>
#include <concepts>
#include <memory>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <glm/glm.hpp>
#include "body.h"
#define RAPIDJSON_HAS_STDSTRING 1
#include "../lib/rapidjson/include/rapidjson/document.h"
#include <mutex>
#include <chrono>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <GL/glew.h>
#include <GL/gl.h> // Will drag system OpenGL headers
#include <cstdio>
#include "../lib/rapidjson/include/rapidjson/document.h"
#include "../lib/rapidjson/include/rapidjson/writer.h"
#include "../lib/rapidjson/include/rapidjson/stringbuffer.h"
struct World;

/**
 * The most basic game object
 */

class Actor {
protected:
    std::string name;
    std::shared_ptr<PhysicsBody> body;

public:
    Actor() = default;
    Actor(std::string name, std::shared_ptr<PhysicsBody> body);
    virtual ~Actor() = default;

    /**
     * AI tick function. Called each frame
     */
    virtual void update(float delta, const std::shared_ptr<World>& world);
    virtual void physics(float delta, const std::shared_ptr<World>& world);

    inline const std::string& getName() const {
        return this->name;
    }

    inline void setName(const std::string& name) {
        this->name = name;
    }

    inline const PhysicsBody& getBody() const {
      return *body;
    }
    glm::vec2 size() const {
        return glm::vec2(body->size().x, body->size().y);
    }

    const PhysicsBody& getRect() const {
      	return *body.get();
    }

    inline glm::vec2 getPos() const {
        return this->body->pos();
    }

    inline void setPosition(const glm::vec2& pos) {
        this->body->setPos(pos);
    }
    void operator+=(glm::vec2 other){
        *body += other;
    }
    virtual void render(ImDrawList* list, glm::vec2 Camera, float scale, float windowWidth, float windowHeight){}
    /**
     * {
     *  "name": "...",
     *  "pos": {
     *    "x": 0.0,
     *    "y": 0.0
     *  }
     * }
     */
    std::string toJSON();

    rapidjson::Value toJSONObject();
    void fromJSON(const rapidjson::Value& value);
};
class Player; 
class Enemy; 
class Npc; 
// ZA WARUDO, im sorry
struct World{
    // Static Bodies, aka things that wont move (walls and stuff like that)
    std::vector<std::shared_ptr<PhysicsBody>> staticBodies; 
    // Rigid Bodies, aka things that might move (players)
    std::vector<std::shared_ptr<PhysicsBody>> rigidBodies; 
    // Hitboxes for Players
    std::vector<std::shared_ptr<Player>> players;
    // Hitboxes for Enemies
    std::vector<std::shared_ptr<Enemy>> enemies;
    // Hitboxes for NPCS
    std::vector<std::shared_ptr<Npc>> npcs;

    // ActorManager
    std::unordered_map<std::string, std::shared_ptr<Actor>> actors;
    std::mutex muntex;
    
    /**
     * Create a new Actor.
     * @param args Constructor arguments for that Actor
     * @param <T> Actor type. It MUST inherit Actor
     * @example auto enemy = Actor::create<M1ChipEnemy>(x, y);
     */

    void removeActor(const std::string& name);
    
    /**
     * Check for any Actors that should have been deleted that haven't been.
     * Don't use in final build, this will unnecessarily slow things down.
     */
    void checkForDanglingActors();

    // returns true if point is colliding with wall
    static bool checkCollisions(const std::vector<std::shared_ptr<PhysicsBody>>& space, const glm::vec2 point, const glm::vec2 size){
      for (auto& s: space){
        if (s->isColliding(point, size)){
          return true;
        }
      }
      return false;
    }
    // returns true if body is colliding with wall
    static bool checkCollisions(const std::vector<std::shared_ptr<PhysicsBody>>& space, const PhysicsBody& body){
      for (auto& s: space){
        if (s->isColliding(body)){
          return true;
        }
      }
      return false;
    } 

    void update(){
      float delta;
      std::shared_ptr<World> ptr(this);
      auto start = std::chrono::system_clock::now(); 
      auto end = start;
      while(true){
        start = std::chrono::system_clock::now(); 
        delta = std::chrono::duration<float, std::milli>(start - end).count();
        end = start;
        muntex.lock();
      for (auto& actor: actors){
        actor.second->update(delta, ptr);
        }
        muntex.unlock();
      }
    }

    void physics(){
      float delta;
      std::shared_ptr<World> ptr(this);
      auto start = std::chrono::system_clock::now(); 
      auto end = start;
      float timer = 0;
        std::cout << "thread\n";
      while(true){
        start = std::chrono::system_clock::now(); 
        delta = std::chrono::duration<float, std::milli>(start - end).count();
        end = start;
        if (timer > 100){
        muntex.lock();
      for (auto& actor: actors){
        actor.second->physics(delta, ptr);
        }
        muntex.unlock();
          timer = 0;
      } 
      timer += delta;
      }
    }



    std::string toJSON();
    void fromJSON(const rapidjson::Value& json);
};

