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

class ActorManager;

/**
 * The most basic game object
 */
class Actor {
private:
    std::string name;
    int64_t x;
    int64_t y;

protected:
    Actor(const std::string name = "Actor", int64_t x = 0, int64_t y = 0);
    
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

    inline int64_t getX() const {
        return this->x;
    }

    inline void setX(int64_t x) {
        this->x = x;
    }

    inline int64_t getY() const {
        return this->y;
    }

    inline void setY(int64_t y) {
        this->y = y;
    }

    inline void setPosition(int64_t x, int64_t y) {
        this->x = x;
        this->y = y;
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
