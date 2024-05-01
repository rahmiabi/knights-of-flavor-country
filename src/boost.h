/**
 * @purpose Boost item class definition
 * @author flakey5
 */

#pragma once

#include "item.h"
#include "csv_file.h"

/**
 * Health boosts
 */
class Boost : public Item {
private:
    Boost();

    static Boost fromCsvEntry(const CsvEntry& entry);

    friend class ItemRegistry;
    
public:
    // TODO
};
