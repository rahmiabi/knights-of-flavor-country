/**
 * @purpose Ability item class definition
 * @author flakey5
 */

#pragma once

#include "item.h"
#include "csv_file.h"

class ItemRegistry;

/**
 * Player abilities
 */
class Ability : public Item {
private:
    Ability();

    static Ability fromCsvEntry(const CsvEntry& entry);

    friend class ItemRegistry;

public:
    // TODO
};
