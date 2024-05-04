/**
 * @purpose Boost item class definition
 * @author flakey5
 */

#pragma once

#include <stddef.h>
#include <cstdint>
#include <string>

class Item {
protected:
    std::string name = "";
    std::string description;
    /**
     * Amount that exist
     */
    uint32_t amount;

    Item(const std::string& name, const std::string& description);

public:
    Item() = default;
    virtual ~Item() = default;

    /**
     * @returns the max amount that a player is allowed to have of this item
     */
    virtual uint32_t getMaxAmount() {
        return 256;
    }

    const std::string& getName() const {
        return this->name;
    }

    const std::string& getDescription() const {
        return this->description;
    }
};
