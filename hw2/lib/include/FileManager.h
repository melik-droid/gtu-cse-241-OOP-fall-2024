#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <string>
#include <memory>
#include "Cell.h"
#include "MyVector.h"

class FileManager {
public:
    static bool saveToFile(const std::string& filename, const MyVector<MyVector<std::shared_ptr<Cell>>>& data);
    static bool loadFromFile(const std::string& filename, MyVector<MyVector<std::shared_ptr<Cell>>>& data);
};

#endif