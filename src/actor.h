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
#include "glm/vec2.hpp"
#include "body.h"

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
    virtual void update(float delta) {}

    inline const std::string& getName() const {
        return this->name;
    }

    inline void setName(const std::string& name) {
        this->name = name;
    }

    inline glm::vec2 getPos() const {
        return this->body->pos();
    }

    inline void setPosition(const glm::vec2& pos) {
        this->body->setPos(pos);
    }
};

// ZA WARUDO, im sorry
struct World{
    // Static Bodies, aka things that wont move (walls and stuff like that)
    std::vector<std::shared_ptr<PhysicsBody>> staticBodies; 
    // Rigid Bodies, aka things that might move (players)
    std::vector<std::shared_ptr<PhysicsBody>> rigidBodies; 
    // ActorManager
    std::unordered_map<std::string, std::shared_ptr<Actor>> actors;
    
    /**
     * Create a new Actor.
     * @param args Constructor arguments for that Actor
     * @param <T> Actor type. It MUST inherit Actor
     * @example auto enemy = Actor::create<M1ChipEnemy>(x, y);
     */
    template<typename T, typename... Args>
    std::shared_ptr<T> createActor(Args&&... args) requires std::is_base_of<Actor, T>::value {
        std::shared_ptr<T> actor(new T(std::forward<Args>(args)...));
        actors[actor->getName()] = actor;
        return actor;
    }

    void removeActor(const std::string& name);
    
    /**
     * Check for any Actors that should have been deleted that haven't been.
     * Don't use in final build, this will unnecessarily slow things down.
     */
    void checkForDanglingActors();

    // returns true if point is colliding with wall
    bool checkCollisions(const std::vector<std::shared_ptr<PhysicsBody>>& space, const glm::vec2 point, const glm::vec2 size){
      for (auto& s: space){
        if (s->isColliding(point, size)){
          return true;
        }
      }
      return false;
    }
    // returns true if body is colliding with wall
    bool checkCollisions(const std::vector<std::shared_ptr<PhysicsBody>>& space, const PhysicsBody& body){
      for (auto& s: space){
        if (s->isColliding(body)){
          return true;
        }
      }
      return false;
    } 
} world;

