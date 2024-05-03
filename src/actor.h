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
#define RAPIDJSON_HAS_STDSTRING true
#include "../lib/rapidjson/include/rapidjson/document.h"
#include <chrono>

struct World;

/**
 * The most basic game object
 */

class Actor {
protected:
    std::string name;
    std::shared_ptr<PhysicsBody> body;

    friend struct World;

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

    inline glm::vec2 getPos() const {
        return this->body->pos();
    }

    inline void setPosition(const glm::vec2& pos) {
        this->body->setPos(pos);
    }
    void operator+=(glm::vec2 other){
        *body += other;
    }

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
// ZA WARUDO, im sorry
struct World{
    // Static Bodies, aka things that wont move (walls and stuff like that)
    std::vector<std::shared_ptr<PhysicsBody>> staticBodies; 
    // Rigid Bodies, aka things that might move (players)
    std::vector<std::shared_ptr<PhysicsBody>> rigidBodies; 
    // ActorManager
    std::unordered_map<std::string, std::shared_ptr<Actor>> actors;
    std::vector<std::shared_ptr<Player>> players;
    
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
      for (auto& actor: actors){
        actor.second->update(delta, ptr);
        }
      }
    }

    void physics(){
      float delta;
      std::shared_ptr<World> ptr(this);
      auto start = std::chrono::system_clock::now(); 
      auto end = start;
      while(true){
        start = std::chrono::system_clock::now(); 
        delta = std::chrono::duration<float, std::milli>(start - end).count();
        end = start;
      for (auto& actor: actors){
        actor.second->physics(delta, ptr);
        }
      } 
    }



    std::string toJSON();
    void fromJSON(const rapidjson::Value& json);
};

