#include "inventory.h"

#include <stdexcept>

size_t Inventory::getItemCount(ItemId id) {
    if (!this->items.contains(id)) {
        throw std::runtime_error("Inventory::getItemCount: inventory doesn't contain item");
    }
    return this->items[id];
}

/**
 * @returns true if a player has the item
 */
bool Inventory::hasItem(ItemId id) {
    return this->items.contains(id);
}

size_t Inventory::addItem(ItemId id, uint8_t amount) {
    // TODO check overflow
    this->items[id] += amount;
}

size_t Inventory::removeItem(ItemId id, uint8_t amount) {
    // TODO check overflow
    this->items[id] -= amount;
}
