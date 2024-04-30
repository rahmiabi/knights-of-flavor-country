#include "csv_file.h"

#include <fstream>
#include <sstream>

std::vector<std::string> splitString(const std::string& s, char c) {
    std::vector<std::string> tokens;

    std::string token;
    std::istringstream stream(s);
    while (std::getline(stream, token, c)) {
        tokens.push_back(token);
    }
    
    return tokens;
}

CsvEntry::CsvEntry(std::vector<std::string>&& tokens, CsvIndexMap& indexMap) : tokens(tokens), indexMap(indexMap) {
}

bool CsvEntry::hasColumn(const std::string& column) const {
    if (!this->indexMap.contains(column)) {
        return false;
    }

    size_t index = this->indexMap[column];
    return this->tokens.size() > index;
}

const std::string& CsvEntry::getValueAtColumn(const std::string& column) const {
    if (!this->indexMap.contains(column)) {
        throw std::runtime_error("CsvEntry::getValueAtColumn column doesn't exist");
    }

    size_t index = this->indexMap[column];
    if (this->tokens.size() <= index) {
        throw std::runtime_error("CsvEntry::getValueAtColumn column doesn't exist");
    }

    return this->tokens[index];
}

CsvFile::CsvFile(const std::string& fileName) {
    std::ifstream in(fileName);
    if (!in.is_open()) {
        this->parseError = true;
        return;
    }

    std::string line;
    bool isFirstLine = true;
    while (std::getline(in, line)) {
        if (isFirstLine) {
            // Build the indexMap
            isFirstLine = false;
            
            std::vector<std::string> schema = splitString(line, ',');
            for (size_t i = 0; i < schema.size(); i++) {
                this->indexMap[schema[i]] = i;
            }

            continue;
        }

        // Skip empty lines and comments
        if (line.length() == 0 || line.starts_with("#")) {
            continue;
        }

        this->lines.push_back(CsvEntry(splitString(line, ','), this->indexMap));
    }
}
