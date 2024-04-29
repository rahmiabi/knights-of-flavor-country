#pragma once

#include <stddef.h>
#include <string>

using ItemId = size_t;

class Item {
protected:
    std::string name;

    Item(const std::string& name);

public:
    virtual ~Item() = default;
    
    std::string& getName() {
        return this->name;
    }

    const std::string& getName() const {
        return this->name;
    }
};

class ItemRegistry {
public:
    static void init();

    static const Item& getItem(ItemId id);
};
