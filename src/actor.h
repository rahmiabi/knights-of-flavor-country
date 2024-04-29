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

class ActorManager;

/**
 * The most basic game object
 */
class Actor {
private:
    std::string name;
    glm::vec2 pos;

protected:
    Actor(const std::string name = "Actor", const glm::vec2& pos = {});
    
    /**
     * AI tick function. Called each frame
     */
    virtual void update(float delta) {}

    friend class ActorManager;

public:
    virtual ~Actor() = default;


    inline const std::string& getName() const {
        return this->name;
    }

    inline void setName(const std::string& name) {
        this->name = name;
    }

    inline float getX() const {
        return this->pos.x;
    }

    inline void setX(float x) {
        this->pos.x = x;
    }

    inline float getY() const {
        return this->pos.y;
    }

    inline void setY(float y) {
        this->pos.y = y;
    }

    inline glm::vec2& getPosition() {
        return this->pos;
    }

    inline const glm::vec2& getPosition() const {
        return this->pos;
    }

    inline void setPosition(const glm::vec2& pos) {
        this->pos = pos;
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
public:
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
