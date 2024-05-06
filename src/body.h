#pragma once

#include <glm/vec2.hpp>
#include "rapidjson/document.h"

enum class Shape : int32_t {
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

    rapidjson::Value toJSONObject(rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>& allocator) {
        rapidjson::Value value;
        value.SetObject();

        rapidjson::Value startVecJson;
        startVecJson.SetObject();
        startVecJson.AddMember("x", this->startVec.x, allocator);
        startVecJson.AddMember("y", this->startVec.y, allocator);
        value.AddMember("startVec", startVecJson, allocator);

        rapidjson::Value sizeVecJson;
        sizeVecJson.SetObject();
        sizeVecJson.AddMember("x", this->sizeVec.x, allocator);
        sizeVecJson.AddMember("y", this->sizeVec.y, allocator);
        value.AddMember("sizeVec", sizeVecJson, allocator);

        value.AddMember("shape", static_cast<int32_t>(this->getShape()), allocator);

        return value;
    }

    void fromJSONObject(const rapidjson::Value& value) {
        const auto& startVecJson = value["startVec"].GetObj();
        this->startVec.x = startVecJson["x"].GetFloat();
        this->startVec.y = startVecJson["y"].GetFloat();

        const auto& sizeVecJson = value["sizeVec"].GetObj();
        this->sizeVec.x = sizeVecJson["x"].GetFloat();
        this->sizeVec.y = sizeVecJson["y"].GetFloat();

        this->shape = static_cast<Shape>(value["shape"].GetInt());
    }
};

// Rectangule Static Body Class
class Rect final : public PhysicsBody  {
    Shape shape = Shape::RECTANGLE;

    public:
    // CSTORS
    Rect() = default;
    Rect(glm::vec2 s, glm::vec2 size){
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
//          if (((body.end().x > this->start().x && body.start().x < this->end().x) && 
//              (body.end().y > this->start().y && body.start().y < this->end().y))) 
//                return true;

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
