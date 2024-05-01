#pragma once

#include <glm/vec2.hpp>

enum class Shape {
    RECTANGLE,
    CIRCLE
};

// Static Body Interface, bodies that should not move
class PhysicsBody {
    protected:
    // center of rectangle 
    glm::vec2 startVec;
    // top right of ructanel
    glm::vec2 sizeVec;
      Shape shape;
    public:
    PhysicsBody() = default;
    virtual ~PhysicsBody() = default;

    virtual bool isColliding(glm::vec2 point, glm::vec2 size) const = 0;
    virtual bool isColliding(const PhysicsBody& body) const = 0;

    virtual glm::vec2 start() const = 0;
    virtual glm::vec2 end() const = 0;

    virtual Shape getShape() const = 0;
    virtual glm::vec2 pos() const = 0;
    virtual glm::vec2 size() const = 0;
    virtual void setPos(const glm::vec2& newPos) = 0;
    void operator+=(glm::vec2 other){
        startVec.x += other.x;
        startVec.y += other.y;
    }
};

// Rectangule Static Body Class
class Rectangle final : public PhysicsBody  {
    Shape shape = Shape::RECTANGLE;

    public:
    // CSTORS
    Rectangle() = default;
    Rectangle(glm::vec2 s, glm::vec2 size){
        startVec = s;
        sizeVec = size;
    }

    //Checks if point is inside the body
    bool isColliding(glm::vec2 point, glm::vec2 size) const override {
        if (((point.x + size.x > this->start().x && point.x - size.x < this->end().x) && 
            (point.y + size.y > this->start().y && point.y - size.y < this->end().y))) 
                return true;
        return false;
    }

    bool isColliding(const PhysicsBody& body) const override {
          if (((body.end().x > this->start().x && body.start().x < this->end().x) && 
              (body.end().y > this->start().y && body.start().y < this->end().y))) 
                return true;
          else return false;
    }


    // getters
    glm::vec2 start() const override {
        return glm::vec2(startVec.x - sizeVec.x / 2, startVec.y - sizeVec.y / 2);
    }

    glm::vec2 end() const override{
        return glm::vec2(startVec.x + sizeVec.x / 2, startVec.y + sizeVec.y / 2);
    }

    glm::vec2 size() const override {
        return sizeVec;
    }

    glm::vec2 pos() const override {
        return startVec;
    }
    void setPos(const glm::vec2& newPos) override {
	startVec = newPos;
    }

    Shape getShape() const override {
	 return shape;
    }

};

// TODO - ADD CIRCLE COLLISIONS
class Circle final : public PhysicsBody {


};