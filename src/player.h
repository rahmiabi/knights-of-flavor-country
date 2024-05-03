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
    double scale = 0.5;
    GLuint texture;
    // stores pos
    
public:
    Player() = default;
    ~Player() = default;

    Player(glm::vec2 position, glm::vec2 size, auto& newTexture){
        body = std::shared_ptr<PhysicsBody>(new Rect(position, glm::vec2(size.x * scale, size.y * scale)));
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

    void render(ImDrawList* list, glm::vec2 Camera, float scale, float windowWidth, float windowHeight){
        list->AddImage((void*) texture, ImVec2((body->start().x - Camera.x) * scale + windowWidth / 2, (body->start().y - Camera.y) * scale + windowHeight / 2) , 
                                        ImVec2((body->end().x - Camera.x) * scale + windowWidth / 2, (body->end().y - Camera.y) * scale + windowHeight / 2) , ImVec2(0,0) , ImVec2(1, 1) , IM_COL32(255, 255, 255, 255));
    }
};
