#pragma once

#include "actor.h"
#include "body.h"
#include <glm/glm.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <iostream>
#include <GL/glew.h>
#include <GL/gl.h> // Will drag system OpenGL headers


class Player final : public Actor {
private:
    double scale = 0.1;
    GLuint texture;
    glm::vec2 textureDimensions;
    Rectangle rect/*um*/; 
    
public:
    Player() = default;
    ~Player() = default;

    Player(glm::vec2 position, glm::vec2 size, auto& newTexture){
        rect = Rectangle(position, size);
        texture = newTexture;
        textureDimensions = size;
    }

    glm::vec2 pos() const {
        return rect.pos();
    }
    glm::vec2 size() const {
        return glm::vec2(rect.size().x * scale, rect.size().y * scale);
    }

    void operator+=(glm::vec2 other){
        std::cout << other.x << " " << other.y << "\n";
        rect += other;
    }
    void render(ImDrawList* list, glm::vec2 Camera){
        list->AddImage((void*) texture, ImVec2(textureDimensions.x * scale + this->pos().x - textureDimensions.y * scale / 2 - Camera.x, textureDimensions.y * scale + this->pos().y - textureDimensions.y * scale /2 - Camera.y) , 
                                        ImVec2(this->pos().x - textureDimensions.x * scale / 2 - Camera.x, this->pos().y - textureDimensions.y * scale / 2 - Camera.y) , ImVec2(1,1) , ImVec2(0, 0) , IM_COL32(255, 255, 255, 255));
    }
};
