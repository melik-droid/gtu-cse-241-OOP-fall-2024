#include <fstream>
#include <string>
#include <sstream>
#include <memory>

#include "FileManager.h"
#include "MyVector.h"
#include "FormulaParser.h"
#include "Constants.h"

using std::string;
using std::shared_ptr;

bool FileManager::saveToFile(const string& filename, const MyVector<MyVector<shared_ptr<Cell>>>& data) {
    std::ofstream file(filename);
    if(!file.is_open()) 
        return false;

    // Write header
    file << "row,column,type,value\n";

    // Write non-empty cells
    for(size_t row = 0; row < data.size(); row++) {
        for(size_t col = 0; col < data[row].size(); col++) {
            const shared_ptr<Cell>& cell = data[row][col];
            string cellType = cell->getType();
            string value = cell->getValue();

            if (cellType != "") {
                char typeChar;
                if (cellType == "(F)")
                    typeChar = 'F';
                else if (cellType == "(V)")
                    typeChar = 'V';
                else if (cellType == "(L)")
                    typeChar = 'L';
                else
                    continue;

                file << row << "," << col << "," << typeChar << ",";
                if(cellType == "(F)") {
                    auto formulaCell = std::dynamic_pointer_cast<FormulaCell>(cell);
                    file << formulaCell->getFormula();
                } else{
                    file << value;
                }
                file << "\n";
            }
        }
    }
    
    file.close();
    return true;
}

bool FileManager::loadFromFile(const string& filename, MyVector<MyVector<shared_ptr<Cell>>>& data) {
    std::ifstream file(filename);
    if (!file.is_open()) 
        return false;

    // Clear existing data and initialize with empty cells
    data.clear();
    for (int i = 0; i < Constants::TOTAL_ROWS; i++) {
        MyVector<shared_ptr<Cell>> row(Constants::TOTAL_COLS, std::make_shared<ValueCell>());
        data.push_back(std::move(row));
    }

    string line;
    getline(file, line); // Skip header

    while (getline(file, line)) {
        std::stringstream ss(line);
        string token;
        MyVector<string> tokens;

        // Split line into tokens
        while (getline(ss, token, ',')) {
            tokens.push_back(token);
        }

        if (tokens.size() != 4) continue;

        // Parse row and column
        size_t row = std::stoul(tokens[0]);
        size_t col = std::stoul(tokens[1]);
        char type = tokens[2][0];
        string value = tokens[3];

        // Create and assign appropriate cell type
        shared_ptr<Cell> cell;
        switch (type) {
            case 'F': {
                auto formulaCell = std::make_shared<FormulaCell>();
                formulaCell->setFormula(value);
                formulaCell->setValue(value);
                cell = formulaCell;
                break;
            }
            case 'V': {
                if (value.find('.') != string::npos) {
                    auto doubleCell = std::make_shared<DoubleValueCell>();
                    doubleCell->setValue(value);
                    cell = doubleCell;
                } else {
                    auto intCell = std::make_shared<IntValueCell>();
                    intCell->setValue(value);
                    cell = intCell;
                }
                break;
            }
            case 'L': {
                auto stringCell = std::make_shared<StringValueCell>();
                stringCell->setValue(value);
                cell = stringCell;
                break;
            }
            default:
                continue;
        }

        // Ensure data vector has enough capacity
        while (data.size() <= row) {
            data.push_back(MyVector<shared_ptr<Cell>>(Constants::TOTAL_COLS, std::make_shared<ValueCell>()));
        }
        while (data[row].size() <= col) {
            data[row].push_back(std::make_shared<ValueCell>());
        }

        data[row][col] = cell;
    }

    file.close();
    return true;
}
