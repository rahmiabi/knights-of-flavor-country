/**
 * @purpose Actor class definition
 * @author flakey5 (Aaron)
 */

#pragma once

#include <cstdint>
#include <string>
#include <concepts>
#include <memory>
#include <unordered_map>
#include "glm/vec2.hpp"
#include "body.h"

class ActorManager;

/**
 * The most basic game object
 */
class Actor {
protected:
    std::string name;
    std::shared_ptr<PhysicsBody> body;
    /**
     * AI tick function. Called each frame
     */
    virtual void update(float delta) {}

    friend class ActorManager;

public:
    Actor() = default;
    Actor(std::string name, std::shared_ptr<PhysicsBody> body);
    virtual ~Actor() = default;


    inline const std::string& getName() const {
        return this->name;
    }

    inline void setName(const std::string& name) {
        this->name = name;
    }

    inline glm::vec2 getPos() const {
        return this->body->pos();
    }

//    inline void setX(float x) {
//        this->pos.x = x;
//    }
//
//
//    inline void setY(float y) {
//        this->pos.y = y;
//    }


//    inline const glm::vec2& getPosition() const {
//        return this->pos;
//    }

    inline void setPosition(const glm::vec2& pos) {
        this->body->setPos(pos);
    }
};

/**
 * They want us to keep track of all the instances of Actor in std::shared_ptr's,
 *  so here's that.
 * 
 * Usage:
 *  auto actor = ActorManager::create<ActorType>(<ActorType ctor options here>);
 * 
 * For example, this creates an M1ChipEnemy at coords (0,0):
 *  std::shared_ptr<M1ChipEnemy> actor = ActorManager::create<M1ChipEnemy>(0, 0);
 * 
 * When you're ready to delete an Actor,
 *  ActorManager::remove("actor name");
 */
class ActorManager {
private:
    static std::unordered_map<std::string, std::shared_ptr<Actor>> sActors;

public:
    /**
     * Create a new Actor
     * @example auto enemy = Actor::create<M1ChipEnemy>(x, y);
     */
    template<typename T, typename... Args>
    static std::shared_ptr<T> create(Args&&... args) requires std::is_base_of<Actor, T>::value {
        std::shared_ptr<T> actor(new T(std::forward<Args>(args)...));
        sActors[actor->getName()] = actor;
        return actor;
    }

    /**
     * Remove an actor from the global list.
     */
    static void remove(const std::string& name);

    /**
     * Call each of the Actor's update() function
     */
    static void update(float delta);

    /**
     * Check for any Actors that should have been deleted that haven't been.
     * Don't use in final build, this will unnecessarily slow things down.
     */
    static void checkForDanglingActors();
};

// ZA WARUDO, im sorry
struct World{
  // Static Bodies, aka things that wont move (walls and stuff like that)
  vector<shared_ptr<PhysicsBody>> staticBodies; 
  // Rigid Bodies, aka things that might move (players)
  vector<shared_ptr<PhysicsBody>> rigidBodies; 
  // ActorManager
  // returns true if point is colliding with wall
  bool checkCollisions(const vector<shared_ptr<PhysicsBody>>& space, const glm::vec2 point, const glm::vec2 size){
    for (auto& s: space){
      if (s->isColliding(point, size)){
        return true;
      }
    }
    return false;
  }
  // returns true if body is colliding with wall
  bool checkCollisions(const vector<shared_ptr<PhysicsBody>>& space, const PhysicsBody& body){
    for (auto& s: space){
      if (s->isColliding(body)){
        return true;
      }
    }
    return false;
  } 
  ActorManager actors;
};

