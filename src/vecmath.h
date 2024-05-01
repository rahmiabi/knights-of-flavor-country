// functions for vector math
#pragma once

#include <glm/glm.hpp>

// normalizes vector
void normalize(glm::vec2 &thing){
        if(thing.x == 0 && thing.y == 0) return;
        float mag = sqrt(pow(thing.x, 2) + pow(thing.y,2));
        thing.x /= mag;
        thing.y /= mag;
}
