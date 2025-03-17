#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <string>
#include <vector>
#include "Cell.h"

class FileManager {
public:
    static bool saveToFile(const std::string& filename, const std::vector<std::vector<Cell>>& data);
    static bool loadFromFile(const std::string& filename, std::vector<std::vector<Cell>>& data);
};

#endif