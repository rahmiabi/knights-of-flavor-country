#include "item.h"

#include <stdexcept>
#include <map>

static std::map<ItemId, Item> gItemMap;

Item::Item(const std::string& name) {
    this->name = name;
}

void ItemRegistry::init() {
    // TODO registers item
}

const Item& ItemRegistry::getItem(ItemId id) {
    if (!gItemMap.contains(id)) {
        throw std::runtime_error("ItemRegistry::getItem item doesn't exist");
    }
}
