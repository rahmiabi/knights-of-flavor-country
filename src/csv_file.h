/**
 * Utility for parsing .csv files
 * @author flakey5
 */

#pragma once

#include <cassert>
#include <vector>
#include <string>
#include <unordered_map>

using CsvIndexMap = std::unordered_map<std::string, size_t>;

class CsvFile;

#define ASSERT_COLUMN(entry, name) if (!entry.hasColumn(name)) throw std::runtime_error("entry " name " missing from " #entry " (" __FILE__ ")");

/**
 * A specific line in a .csv file
 */
class CsvEntry {
public:
    std::vector<std::string> tokens;
    CsvIndexMap& indexMap;

    CsvEntry(std::vector<std::string>&& tokens, CsvIndexMap& indexMap);

    friend class CsvFile;

public:
    /**
     * @returns true if the 
     */
    bool hasColumn(const std::string& column) const;

    /**
     * @returns the value of a column
     * @throws std::runtime_error if column doesn't exist
     */
    const std::string& getValueAtColumn(const std::string& column) const;

    inline const std::string& operator[](const std::string& column) const {
        return this->getValueAtColumn(column);
    }

    uint32_t getU32(const std::string& column) const;
};

class CsvFile {
private:
    /**
     * Maps column name->index in token map
     */
    CsvIndexMap indexMap;
    std::vector<CsvEntry> lines;
    bool parseError = false;

public:
    CsvFile(const std::string& fileName);

    inline bool hasParseError() {
        return this->parseError;
    }

    inline bool hasColumn(const std::string& column) const {
        return this->indexMap.contains(column);
    }

    inline auto begin() {
        assert(!parseError);
        return this->lines.begin();
    }

    inline auto begin() const {
        assert(!parseError);
        return this->lines.begin();
    }
    
    inline auto end() {
        assert(!parseError);
        return this->lines.end();
    }

    inline auto end() const {
        assert(!parseError);
        return this->lines.end();
    }
};
