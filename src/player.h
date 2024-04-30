#pragma once

#include "actor.h"
#include "body.h"
#include <glm/glm.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <GL/glew.h>
#include <GL/gl.h> // Will drag system OpenGL headers

#include "attack_traits.h"
#include <memory>

class Player final : public Actor, AttackTraits {
private:
    double scale = 0.1;
    GLuint texture;
    // stores pos
    
public:
    Player() = default;
    ~Player() = default;

    Player(glm::vec2 position, glm::vec2 size, auto& newTexture){
        body = std::shared_ptr<PhysicsBody>(new Rectangle(position, glm::vec2(size.x * scale, size.y * scale)));
        texture = newTexture;
    }

    glm::vec2 pos() const {
        return body->pos();
    }
    glm::vec2 size() const {
        return glm::vec2(body->size().x, body->size().y);
    }

    const PhysicsBody& getRect() const {
      	return *body.get();
    }

    void operator+=(glm::vec2 other){
        *body += other;
    }
    void render(ImDrawList* list, glm::vec2 Camera){
        list->AddImage((void*) texture, ImVec2(body->size().x + this->pos().x - body->size().y / 2 - Camera.x, body->size().y + this->pos().y - body->size().y /2 - Camera.y) , 
                                        ImVec2(this->pos().x - body->size().x / 2 - Camera.x, this->pos().y - body->size().y / 2 - Camera.y) , ImVec2(1,1) , ImVec2(0, 0) , IM_COL32(255, 255, 255, 255));
    }
};
