#pragma once

#include "actor.h"
#include "square.h"

class Player final : public Actor {
private:
    Square area = {};

public:
    inline const Square& getArea() const {
        return this->area;
    }

    inline Square& getArea() {
        return this->area;
    }
};
