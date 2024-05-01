#include "item.h"

#include <stdexcept>
#include <map>

Item::Item(const std::string& name, const std::string& description) {
    this->name = name;
    this->description = description;
}
