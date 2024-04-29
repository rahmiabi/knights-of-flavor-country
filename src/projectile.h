#pragma once

#include "glm/vec2.hpp"

struct Projectile {
    glm::vec2 pos;
    glm::vec2 vel;
    glm::vec2 end;
    bool render = true;
};
