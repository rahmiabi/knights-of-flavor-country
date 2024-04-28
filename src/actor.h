#pragma once

#include <string>

/**
 * The most basic game object
 */
class Actor {
private:
    std::string name;
    int x;
    int y;

public:
    Actor(const std::string name = "Actor", int x = 0, int y = 0);
    virtual ~Actor() = default;

    inline const std::string& getName() const {
        return this->name;
    }

    inline void setName(const std::string& name) {
        this->name = name;
    }

    inline int getX() const {
        return this->x;
    }

    inline void setX(int x) {
        this->x = x;
    }

    inline int getY() const {
        return this->y;
    }

    inline void setY(int y) {
        this->y = y;
    }
};
