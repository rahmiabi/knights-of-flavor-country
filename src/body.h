#pragma once

#include <glm/vec2.hpp>

// Static Body Interface, bodies that should not move
class PhysicsBody {
    public:
    PhysicsBody() = default;
    virtual ~PhysicsBody() = default;

    virtual bool isColliding(glm::vec2 point, glm::vec2 size) const = 0;

    virtual glm::vec2 start() const = 0;
    virtual glm::vec2 end() const = 0;
};

// Rectangule Static Body Class
class Rectangle final : public PhysicsBody  {
    // center of rectangle 
    glm::vec2 startVec;
    // top right of ructanel
    glm::vec2 sizeVec;

    public:
    // CSTORS
    Rectangle() = default;
    Rectangle(glm::vec2 s, glm::vec2 size){
        startVec = s;
        sizeVec = size;
    }

    //Checks if point is inside the body
    bool isColliding(glm::vec2 point, glm::vec2 size) const override {
        if (((point.x + size.x > startVec.x - sizeVec.x && point.x - size.x < startVec.x + sizeVec.x) && 
            (point.y + size.y > startVec.y - sizeVec.y && point.y - size.y < startVec.y + sizeVec.y))) 
                return true;
        return false;
    }

    // getters
    glm::vec2 start() const override {
        return startVec - sizeVec;
    }

    glm::vec2 end() const override{
        return startVec + sizeVec;
    }

    glm::vec2 size() const {
        return sizeVec;
    }

    glm::vec2 pos() const {
        return startVec;
    }

    void operator+=(glm::vec2 other){
        startVec.x += other.x;
        startVec.y += other.y;
    }
};
