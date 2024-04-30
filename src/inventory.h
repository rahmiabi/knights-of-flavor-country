/**
 * @purpose Inventory class definitions
 * @author flakey5 (Aaron)
 */

#pragma once

#include <cstdint>
#include <map>
#include "item.h"

class Inventory {
private:
    std::map<ItemId, uint8_t> items;

public:
    /**
     * Example: `getItemCount(ITEM_APPLE)` returns the number of apples a player has
     * @returns How many of a specific item a player has
     */
    size_t getItemCount(ItemId id);

    /**
     * @returns true if a player has the item
     */
    bool hasItem(ItemId id);

    /**
     * Adds an item to a player's inventory
     * @param id Item to add
     * @param amount Amount of that item to add
     * @returns Amount of that item the player now has in their inventory
     */
    size_t addItem(ItemId id, uint8_t amount = 1);

    /**
     * Removes an item to a player's inventory
     * @param id Item to add
     * @param amount Amount of that item to remove
     * @returns Amount of that item the player now has in their inventory
     */
    size_t removeItem(ItemId id, uint8_t amount = 1);
};
